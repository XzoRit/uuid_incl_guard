#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>
#include <algorithm>

using namespace std;
using namespace boost;
using namespace boost::uuids;
using namespace boost::xpressive;
using namespace boost::algorithm;

namespace po = boost::program_options;

/*
* TODO:
* - recognize if file has copyright notice
* - accept company name as parameter
* - generate complete default class with .h/.cpp
* - accept many files as parameters
* - accept parameter to print n uuids on console
* - accept parameter to write include guard in a different file/console
*/

string copyright =
"/*\n"
" * Copyright (c) <Company>\n"
" * All rights reserved. Company confidential.\n"
" */\n"
"\n";

string inclGuard =
"#ifndef <Id>\n"
"#define <Id>\n"
"\n";

string const endIf = "\n#endif\n";

bool hasCopyrightNotice(string const& content)
{
	smatch what;
	return regex_search(content, what, sregex(as_xpr("Copyright")));
}

typedef boost::optional<string> MaybeInclGuard;
MaybeInclGuard hasInclGuard(string const& content)
{
	sregex const reIfndef = as_xpr("#ifndef") >> +_s >> (s1 = +_w);
	smatch what;
	if (regex_search(content, what, reIfndef))
		return boost::make_optional(what[1].str());
	else
		return MaybeInclGuard();
}

int main(int argCount, char const* args[])
{
	cout << "uuid_incl_guard\n";

	po::options_description desc("allowed options");
	desc.add_options()
		("help", "produce help message")
		("company", po::value<string>(), "name of company to be placed in copyright notice")
		("in_files", po::value<vector<string> >(), "place include guards in this file");

	po::variables_map vm;
	po::store(po::parse_command_line(argCount, args, desc), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		cout << desc << '\n';
		return 0;
	}

	if (vm.count("company"))
	{
		replace_first(copyright, "<Company>", vm["company"].as<string>());
	}
	else
	{
		cout << desc << '\n';
		return 0;
	}

	if (vm.count("in_files"))
	{
		vector<string> files = vm["in_files"].as<vector<string> >();
		for (auto fileName = files.cbegin(); fileName != files.end(); ++fileName)
		{
			fstream file(*fileName);
			string content((istreambuf_iterator<char>(file)),
				istreambuf_iterator<char>());
			file.seekg(0);

			string id = string("INCL_") + to_string(random_generator()());
			replace(id.begin(), id.end(), '-', '_');

			if (MaybeInclGuard guard = hasInclGuard(content))
			{
				cout << guard.get() << '\n';
				replace_all(content, guard.get(), id);
			}
			else
			{
				replace_all(inclGuard, "<Id>", id);
			}

			if (hasCopyrightNotice(content))
			{
				copyright = "";
			}

			content.insert(0, copyright + inclGuard);
			content.append(endIf);
			cout << content << '\n';
			file << content;
			file.flush();
		}
	}

	return 0;
}
