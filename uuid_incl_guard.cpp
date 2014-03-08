#include "utils.hpp"
#include <boost/program_options.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <string>
#include <streambuf>

using namespace std;
using namespace boost;

namespace po = boost::program_options;
namespace fs = boost::filesystem;

/*
 * TODO:
 * - check for header/source files
 * - check for copyright notice in source files
 */

std::string const copyrightTemplate =
  "/*\n"
  " * Copyright <Company>\n"
  " * All rights reserved. Company confidential.\n"
  " */\n\n";

string const inclGuardTemplate = 
  "#ifndef <Id>\n"   
  "#define <Id>\n"   
  "\n";

string const endIfTemplate = "\n#endif\n";

int main(int argCount, char* args[])
{
  po::options_description desc("usage:\n"
                               "\tuuid_incl_guard [options] files\n"
                               "description:\n"
                               "\t- generates uuids to be used as include guards for headers\n"
                               "\t- puts uuid include guards into header files\n"
                               "\t- puts copyright notice at start of header and source files\n"
                               "options");
  desc.add_options()
    ("help",
     "produce help message")
    ("generate_uuid_include_guards", po::value<unsigned int>(),
     "generate n uuid include guards")
    ("company", po::value<string>(),
     "name of company for copyright notice. "
     "if it is not specified no copyright notice is placed in files")
    ("exchange_uuid", po::value<bool>()->default_value(true),
     "if 1 exchange existing uuid include guards "
     "if 0 do not exchange uuids.")
    ("in_files", po::value<vector<string> >(),
     "place include guards and copyright notices into these files");

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
  if(vm.count("generate_uuid_include_guards"))
    {
      for(unsigned int i = 0; i < vm["generate_uuid_include_guards"].as<unsigned int>(); ++i)
	{
	  cout << generateInclGuard() << '\n';
	}
      return 0;
    }
  if (vm.count("in_files"))
    {
      Paths const paths = makePathsFromStrings(vm["in_files"].as<vector<string> >());
      if(!canReadWriteFiles(paths))
	{
	  return 0;
	}
      if(!areCppSourceFiles(paths))
	{
	  return 0;
	}
      for (PathConstIterator path = paths.cbegin(); path != paths.end(); ++path)
	{
	  fs::fstream file(*path);
	  string content((istreambuf_iterator<char>(file)),
			 istreambuf_iterator<char>());

	  string const inclGuardId = generateInclGuard();
	  if (MaybeInclGuard const guard = hasInclGuard(content))
	    {
	      if(vm["exchange_uuid"].as<bool>() || !isUuidInclGuard(guard.get()))
		{
		  replace_all(content, guard.get(), inclGuardId);
		}
	    }
	  else
	    {
	      string const newInclGuard = replace_all_copy(inclGuardTemplate, "<Id>", inclGuardId);
	      content.insert(0, newInclGuard);
	      content.append(endIfTemplate);
	    }

	  if (!hasCopyrightNotice(content))
	    {
	      if(vm.count("company"))
		{
		  string const newCopyright = replace_first_copy(copyrightTemplate, "<Company>", vm["company"].as<string>());
		  content.insert(0, newCopyright);
		}
	    }

	  cout << content << '\n';
	  file.seekg(0);
	  file << content;
	  file.flush();
	}
      return 0;
    }

  return 0;
}
