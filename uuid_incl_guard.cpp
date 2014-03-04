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

std::string const copyright =
  "/*\n"
  " * Copyright <Company>\n"
  " * All rights reserved. Company confidential.\n"
  " */\n\n";

string const inclGuard = 
  "#ifndef <Id>\n"   
  "#define <Id>\n"   
  "\n";

string const endIf = "\n#endif\n";

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

bool isUuidInclGuard(string const& inclGuard)
{
  xp::sregex const reUuid = xp::as_xpr("INCL") >>
    '_' >> xp::repeat<8>(xp::xdigit) >>
    xp::repeat<3>('_' >> xp::repeat<4>(xp::xdigit)) >>
    '_' >> xp::repeat<8>(xp::xdigit);
  return xp::regex_search(inclGuard, reUuid);
}

string generateInclGuard()
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
    ("exchange_uuid", po::value<bool>()->default_value(true), "exchange existing uuid include guard")
    ("in_files", po::value<vector<string> >(), "place include guards in this file");

  po::positional_options_description p;
  p.add("in_files", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(argCount, args).options(desc).positional(p).run(), vm);
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

          string const inclGuardId = generateInclGuard();
          if (MaybeInclGuard guard = hasInclGuard(content))
            {
	      if(vm["exchange_uuid"].as<bool>() || !isUuidInclGuard(guard.get()))
		{
		  replace_all(content, guard.get(), inclGuardId);
		  cout << (*fileName) << ": changed include guard from " << guard.get() << " to " << inclGuardId << '\n';
		}
	    }
	  else
	    {
	      string const newInclGuard = replace_all_copy(inclGuard, "<Id>", inclGuardId);
	      content.insert(0, newInclGuard);
	      content.append(endIf);
	      cout << (*fileName) << ": gets include guard " << inclGuardId << '\n';
	    }

	  if (!hasCopyrightNotice(content))
	    {
	      if(vm.count("company"))
		{
		  string const newCopyright = replace_first_copy(copyright, "<Company>", vm["company"].as<string>());
		  content.insert(0, newCopyright);
		  cout << (*fileName) << " gets copyright notice\n" << newCopyright << '\n';
		}
	      else
		{
		  cout << (*fileName) << " needs copyright notice but company name is missing" << '\n';
		}
	    }

	  cout << content << '\n';
	  file << content;
	  file.flush();
	}
    }


  return 0;
}
