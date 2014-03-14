#ifndef INCL_11f45e0b_1255_4a8d_a441_81af9f23a33a
#define INCL_11f45e0b_1255_4a8d_a441_81af9f23a33a

#include <boost/optional.hpp>
#include <boost/filesystem.hpp>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>

bool hasCopyrightNotice(std::string const& content);

typedef boost::optional<std::string> MaybeInclGuard;
MaybeInclGuard hasInclGuard(std::string const& content);
bool isUuidInclGuard(std::string const& inclGuard);
std::string generateInclGuard();

typedef std::vector<boost::filesystem::path> Paths;
typedef Paths::const_iterator PathConstIterator;
Paths makePathsFromStrings(std::vector<std::string> const& fileNames);
bool isHeaderFile(boost::filesystem::path const& path);
PathConstIterator partitionByReadWriteFile(Paths& paths);
void addCppFilesFromDirectories(Paths& paths, bool recursive);

static bool isReadWriteCppFile(boost::filesystem::path const& path);
template<typename DirIter>
static Paths readWriteCppFiles(Paths& paths, Paths::const_reference dir)
{
  std::copy_if(DirIter(dir),
	       DirIter(),
	       std::back_inserter(paths),
	       isReadWriteCppFile);
  return paths;
}

#endif
