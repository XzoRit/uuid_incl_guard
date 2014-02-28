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

string const copyright =
"/*\n"
" * Copyright (c) <Company>\n"
" * All rights reserved. Company confidential.\n"
" */\n"
"\n";

string const inclGuard =
"#ifndef <Id>\n"
"#define <Id>\n"
"\n";

string const endIf = "\n#endif\n";

bool hasCopyrightNotice();

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
		("in_files", po::value<vector<string> >(), "place include guards in this file");

	po::variables_map vm;
	po::store(po::parse_command_line(argCount, args, desc), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		cout << desc << '\n';
		return 0;
	}

	string id = string("INCL_") + to_string(random_generator()());
	replace(id.begin(), id.end(), '-', '_');

	if (argCount >= 2)
	{
		string const fileName = args[1];
		fstream file(fileName);
		string content((istreambuf_iterator<char>(file)),
			istreambuf_iterator<char>());
		file.seekg(0);
		if (MaybeInclGuard guard = hasInclGuard(content))
		{
			cout << guard.get() << '\n';
			replace_all(content, guard.get(), id);
		}
		else
		{
			string const inclGuardWithUuid = replace_all_copy(inclGuard, "<Id>", id);
			content.insert(0, copyright + inclGuardWithUuid);
			content.append(endIf);
		}
		cout << content << '\n';
		file << content;
		file.flush();
	}
	else
	{
		cout << id << '\n';
	}

	return 0;
}
