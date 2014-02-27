#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>

using namespace std;
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
  " */\n\n";

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

  if(vm.count("help"))
    {
      cout << desc << '\n';
      return 0;
    }
  if(argCount >= 2)
    {
      string const fileName = args[1];
      fstream file(fileName);
      string content((istreambuf_iterator<char>(file)),
		     istreambuf_iterator<char>());
      xp::sregex const reIfndef = xp::as_xpr("#ifndef ") >> +xp::_w;
      xp::smatch what;
      if(xp::regex_search(content, what, reIfndef))
	{
	  cout << "what[0] = " << what[0] << '\n';
	}
      else
	{
	  string const id = to_string(random_generator()());
	  content.insert(0, copyright +
			 string("#ifndef ")
			 .append(id)
			 .append("\n#define ")
			 .append(id)
			 .append("\n\n"));
	  content.append("\n#endif\n");
	  cout << content << '\n';
	  file.seekg(0);
	  file << content;
	}
    }
  else
    {
      cout << random_generator()() << '\n';
    }

  return 0;
}
