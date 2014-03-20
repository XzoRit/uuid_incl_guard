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

static bool isReadWriteFile(fs::path const& path);
static bool isSourceFile(fs::path const& path);
static bool isCppSourceFile(fs::path const& path);
static Paths extractDirectories(Paths& paths);

static boost::filesystem::path makePathFromString(std::string fileName);

bool hasCopyrightNotice(string const& content)
{
  xp::smatch what;
  xp::sregex const reCopyright = xp::icase(xp::as_xpr("* Copyright (c) ")) >> +~xp::_d >> xp::repeat<4>(xp::_d);
  return xp::regex_search(content, what, reCopyright);
}

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

Paths makePathsFromStrings(vector<string> const& fileNames)
{
  Paths paths(fileNames.size());
  transform(fileNames.begin(), fileNames.end(), paths.begin(), makePathFromString);
  return paths;
}

bool isHeaderFile(boost::filesystem::path const& path)
{
  fs::path const ext = path.extension();
  typedef fs::path::string_type StringType;
  return (ext == StringType(".hpp")) || (ext == StringType(".h"));
}

PathConstIterator partitionByReadWriteFile(Paths& paths)
{
  return partition(paths.begin(), paths.end(), isReadWriteFile);
}

void addCppFilesFromDirectories(Paths& paths, bool recursive)
{
  Paths const dirs = extractDirectories(paths);
  Paths srcs;
  if(recursive) srcs = accumulate(dirs.cbegin(), dirs.cend(), Paths(), readWriteCppFiles<fs::recursive_directory_iterator>);
  else srcs = accumulate(dirs.cbegin(), dirs.cend(), Paths(), readWriteCppFiles<fs::directory_iterator>);
  paths.insert(paths.end(), srcs.begin(), srcs.end());
}

bool isReadWriteCppFile(fs::path const& path)
{
  return isReadWriteFile(path) && isCppSourceFile(path);
}

bool isReadWriteFile(fs::path const& path)
{
  try
    {
      fs::file_status const fstat = fs::status(path);
      if(!fs::is_regular_file(fstat)) return false;

      fs::perms permissions = fstat.permissions();
      if(!((permissions & fs::owner_read) && (permissions & fs::owner_write))) return false;
      return true;
    }
  catch(...)
    {
      return false;
    }
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

