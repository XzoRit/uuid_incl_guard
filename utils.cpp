#include "utils.hpp"
#include <boost/xpressive/xpressive.hpp>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <vector>
#include <algorithm>
#include <numeric>
#include <string>

using namespace std;

namespace xp = boost::xpressive;
namespace fs = boost::filesystem;
namespace uuid = boost::uuids;

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
  string id = string("INCL_") + uuid::to_string(uuid::random_generator()());
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

bool isReadWriteFile(fs::file_status const& fstat)
{
  if(!fs::is_regular_file(fstat)) return false;

  fs::perms permissions = fstat.permissions();
  if(!((permissions & fs::owner_read) && (permissions & fs::owner_write))) return false;

  return true;
}

bool isReadWriteFile(fs::path const& path)
{
  try
    {
      fs::file_status const fstat = fs::status(path);
      return isReadWriteFile(fstat);
    }
  catch(...)
    {
      return false;
    }
}

bool isHeaderFile(boost::filesystem::path const& path)
{
  fs::path const ext = path.extension();
  typedef fs::path::string_type StringType;
  return (ext == StringType(".hpp")) || (ext == StringType(".h"));
}

bool isSourceFile(boost::filesystem::path const& path)
{
  fs::path const ext = path.extension();
  typedef fs::path::string_type StringType;
  return (ext == StringType(".cpp")) || (ext == StringType(".c"));
}

bool isCppSourceFile(fs::path const& path)
{
  return isHeaderFile(path) || isSourceFile(path);
}

bool isReadWriteCppFile(fs::path const& path)
{
  return isReadWriteFile(path) && isCppSourceFile(path);
}

PathConstIterator partitionByReadWriteCppFile(Paths& paths)
{
  return partition(paths.begin(), paths.end(), isReadWriteCppFile);
}

bool isDirectory(fs::path const& path)
{
  return fs::is_directory(path);
}

static Paths extractDirectories(Paths& paths)
{
  PathConstIterator sep = partition(paths.begin(), paths.end(), isDirectory);
  Paths dirs;
  copy(paths.cbegin(), sep, back_inserter(dirs));
  paths.erase(paths.cbegin(), sep);
  return dirs;
}

static Paths readWriteCppFiles(Paths& paths, Paths::const_reference dir)
{
  copy_if(fs::directory_iterator(dir),
	  fs::directory_iterator(),
	  back_inserter(paths),
	  isReadWriteCppFile);
  return paths;
}

void addCppFilesFromDirectories(Paths& paths)
{
  Paths const dirs = extractDirectories(paths);
  Paths const srcs = accumulate(dirs.cbegin(), dirs.cend(), Paths(), readWriteCppFiles);
  paths.insert(paths.end(), srcs.begin(), srcs.end());
}
