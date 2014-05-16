#include "utils.hpp"
#include <boost/program_options.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <streambuf>
#include <sstream>
#include <algorithm>

using namespace std;
using namespace boost;

namespace po = boost::program_options;
namespace fs = boost::filesystem;
namespace date = boost::gregorian;

std::string const copyrightTemplate =
  "/*\n"
  " * Copyright <Year> <Company> and Licensors. All Rights Reserved. Company Confidential.\n"
  " */\n\n";

string const inclGuardTemplate = 
  "#ifndef <Id>\n"   
  "#define <Id>\n"   
  "\n";

string const endIfTemplate = "\n#endif\n";

int main(int argCount, char* args[])
{
  string optCompany = "";
  unsigned int optGenNUuids = 0;
  vector<string> optFiles;
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
    ("generate", po::value<unsigned int>(&optGenNUuids)->default_value(1),
     "generate n uuid include guards")
    ("company", po::value<string>(&optCompany),
     "name of company for copyright notice. "
     "if it is not specified no copyright notice is placed in files")
    ("do_not_exchange_uuid",
     "do not exchange existing uuid include guards")
    ("recursive",
     "scan directories recursively for source files")
    ("in", po::value<vector<string> >(&optFiles),
     "place include guards and copyright notice into these files. "
     "if a directory is given it is scanned for source files and these are processed.");

  po::positional_options_description p;
  p.add("in", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(argCount, args).options(desc).positional(p).run(), vm);
  po::notify(vm);

  bool const optRecursive = vm.count("recursive");
  bool const optExchangeUuid = !vm.count("do_not_exchange_uuid");
  if(vm.count("help"))
    {
      cout << desc << '\n';
      return 0;
    }
  if (vm.count("in"))
    {
      stringstream report;
      Paths paths = makePathsFromStrings(optFiles);
      addCppFilesFromDirectories(paths, optRecursive);
      PathConstIterator sep = partitionByReadWriteFile(paths);
      if(sep != paths.cend())
        {
          cerr << "processing aborted\nthese files are not valid:\n";
          copy(sep, paths.cend(), ostream_iterator<Paths::const_reference>(cerr, "\n"));
          return 1;
        }
      date::greg_year year = date::day_clock::local_day().year();
      string const copyright =
        optCompany.empty() ? "" : replace_first_copy(
						     replace_first_copy(
									copyrightTemplate,
									"<Year>",
									lexical_cast<string>(year)),
                                                     "<Company>",
                                                     optCompany);

      for (PathConstIterator path = paths.cbegin(); path != paths.end(); ++path)
        {
          fs::fstream file(*path);
          string content((istreambuf_iterator<char>(file)),
                         istreambuf_iterator<char>());
          if(isHeaderFile(*path))
            {
              string const inclGuardId = generateInclGuard();
              if (MaybeInclGuard const guard = hasInclGuard(content))
                {
                  if(optExchangeUuid || !isUuidInclGuard(guard.get()))
                    {
                      replace_all(content, guard.get(), inclGuardId);
                      report << (*path) << ": " << "replaced " << guard.get() << " with " << inclGuardId << '\n';
                    }
                }
              else
                {
                  string const newInclGuard = replace_all_copy(inclGuardTemplate,
                                                               "<Id>",
                                                               inclGuardId);
                  content.insert(0, newInclGuard);
                  content.append(endIfTemplate);
                  report << (*path) << ": has new include guard " << inclGuardId << '\n';
                }
            }
          if (!(copyright.empty() || hasCopyrightNotice(content)))
            {
              content.insert(0, copyright);
              report << (*path) << ": has new copyright notice with company " << optCompany << '\n';
            }
          file.seekg(0);
          file << content;
          file.flush();
        }
      cout << report.str();
      return 0;
    }
  if(vm.count("generate"))
    {
      for(unsigned int i = 0; i < optGenNUuids; ++i)
        {
          cout << generateInclGuard() << '\n';
        }
      return 0;
    }

  return 0;
}
