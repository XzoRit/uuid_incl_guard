#ifndef INCL_11f45e0b_1255_4a8d_a441_81af9f23a33a
#define INCL_11f45e0b_1255_4a8d_a441_81af9f23a33a

#include <boost/optional.hpp>
#include <boost/filesystem.hpp>
#include <string>
#include <vector>

bool hasCopyrightNotice(std::string const& content);

typedef boost::optional<std::string> MaybeInclGuard;
MaybeInclGuard hasInclGuard(std::string const& content);

bool isUuidInclGuard(std::string const& inclGuard);

std::string generateInclGuard();

boost::filesystem::path makePathFromString(std::string fileName);

typedef std::vector<boost::filesystem::path> Paths;
typedef Paths::const_iterator PathConstIterator;
Paths makePathsFromStrings(std::vector<std::string> const& fileNames);

bool isReadWriteFile(boost::filesystem::file_status const& fstat);

bool isReadWriteFile(boost::filesystem::path const& path);

bool isHeaderFile(boost::filesystem::path const& path);

bool isSourceFile(boost::filesystem::path const& path);

bool isCppSourceFile(boost::filesystem::path const& path);

bool isReadWriteCppFile(boost::filesystem::path const& path);

PathConstIterator partitionByReadWriteCppFile(Paths& paths);

template<typename DirIter>
static Paths readWriteCppFiles(Paths& paths, Paths::const_reference dir)
{
  copy_if(DirIter(dir),
	  DirIter(),
	  back_inserter(paths),
	  isReadWriteCppFile);
  return paths;
}

void addCppFilesFromDirectories(Paths& paths, bool recursive);

#endif
