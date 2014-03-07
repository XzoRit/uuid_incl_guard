#ifndef INCL_11f45e0b_1255_4a8d_a441_81af9f23a33a
#define INCL_11f45e0b_1255_4a8d_a441_81af9f23a33a

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>
#include <algorithm>
#include <iterator>

using namespace std;
using namespace boost;
using namespace boost::uuids;

namespace xp = boost::xpressive;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

bool hasCopyrightNotice(string const& content)
{
  xp::smatch what;
  return xp::regex_search(content, what, xp::sregex(xp::icase(xp::as_xpr("Copyright"))));
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

fs::path makePathFromString(string fileName)
{
  return fs::path(fileName);
}

typedef vector<fs::path> Paths;
typedef Paths::const_iterator PathConstIterator;
Paths makePathsFromStrings(vector<string> const& fileNames)
{
  Paths paths(fileNames.size());
  transform(fileNames.begin(), fileNames.end(), paths.begin(), makePathFromString);
  return paths;
}

bool isReadWriteFile(fs::path const& path)
{
  fs::file_status const fstat = fs::status(path);
  if(!fs::is_regular_file(fstat)) return false;

  fs::perms permissions = fstat.permissions();
  if(!((permissions & fs::owner_read) && (permissions & fs::owner_write))) return false;

  return true;
}

bool canReadWriteFiles(Paths const& paths)
{
  Paths readWriteFiles;
  copy_if(paths.begin(), paths.end(), back_inserter(readWriteFiles), isReadWriteFile);
  return readWriteFiles.size() == paths.size();
}

bool isCppSourceFile(fs::path const& path)
{
  fs::path ext = path.extension();
  typedef fs::path::string_type StringType;
  return (ext == StringType(".hpp")) || (ext == StringType(".h")) || (ext == StringType(".cpp")) || (ext == StringType(".c"));
}

bool areCppSourceFiles(Paths const& paths)
{
  Paths cppFiles;
  copy_if(paths.begin(), paths.end(), back_inserter(cppFiles), isCppSourceFile);
  return cppFiles.size() == paths.size();
}

#endif
