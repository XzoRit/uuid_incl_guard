#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>
#include <algorithm>

using namespace std;
using namespace boost;
using namespace boost::uuids;

namespace xp = boost::xpressive;
namespace po = boost::program_options;

/*
 * TODO:
 * - recognize if file has an include guard
 * - treat #pragma once as include guard
 * - accept many files as parameters
 * - if no parameters -> print a uuid on console
 * - accept parameter to print n uuids on console
 * - write uuid include guard back to file
 * - accept parameter to write include guard in a different file/console
 */

std::string copyright =
  "/*\n"
  " * Copyright <Company>\n"
  " * All rights reserved. Company confidential.\n"
  " */\n\n";

string inclGuard = 
  "#ifndef <Id>\n"   
  "#define <Id>\n"   
  "\n";

string endIf = "\n#endif\n";

bool hasCopyrightNotice(string const& content)
{
  xp::smatch what;
  return xp::regex_search(content, what, xp::sregex(xp::as_xpr("Copyright")));
}

typedef boost::optional<string> MaybeInclGuard;
MaybeInclGuard hasInclGuard(string const& content)
{
  xp::sregex const reIfndef = xp::as_xpr("#ifndef") >> +xp::_s >> (xp::s1 = +xp::_w);
  xp::smatch what;
  if (xp::regex_search(content, what, reIfndef))
    return boost::make_optional(what[1].str());
  else
    return MaybeInclGuard();
}

string generateIncludeGuardId()
{
  string id = string("INCL_") + to_string(random_generator()());
  replace(id.begin(), id.end(), '-', '_');
  return id;
}

int main(int argCount, char const* args[])
{
  cout << "uuid_incl_guard\n";

  po::options_description desc("allowed options");
  desc.add_options()
    ("help", "produce help message")
    ("company", po::value<string>(), "name of company for copyright notice")
    ("in_files", po::value<vector<string> >(), "place include guards in this file");

  po::variables_map vm;
  po::store(po::parse_command_line(argCount, args, desc), vm);
  po::notify(vm);

  if(vm.count("help"))
    {
      cout << desc << '\n';
      return 0;
    }
  if (vm.count("in_files"))
    {
      vector<string> files = vm["in_files"].as<vector<string> >();
      for (vector<string>::const_iterator fileName = files.cbegin(); fileName != files.end(); ++fileName)
	{
	  fstream file(*fileName);
	  string content((istreambuf_iterator<char>(file)),
			 istreambuf_iterator<char>());
	  file.seekg(0);

	  string const inclGuardId = generateIncludeGuardId();
	  if (MaybeInclGuard guard = hasInclGuard(content))
	    {
	      replace_all(content, guard.get(), inclGuardId);
	      cout << (*fileName) << ": changing include guard from " << guard.get() << " to " << inclGuardId << '\n';
	      inclGuard = "";
	      endIf = "";
	    }
	  else
	    {
	      replace_all(inclGuard, "<Id>", inclGuardId);
	      cout << (*fileName) << ": gets include guard " << inclGuardId << '\n';
	    }

	  if (hasCopyrightNotice(content))
	    {
	      copyright = "";
	    }
	  else
	    {
	      if(vm.count("company"))
		{
		  replace_first(copyright, "<Company>", vm["company"].as<string>());
		  cout << (*fileName) << " gets copyright notice\n" << copyright << '\n';
		}
	      else
		{
		  copyright = "";
		  cout << (*fileName) << " needs copyright notice but company name is missing" << '\n';
		}
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
