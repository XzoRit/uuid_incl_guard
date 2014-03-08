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

bool hasCopyrightNotice(string const& content);

typedef boost::optional<string> MaybeInclGuard;
MaybeInclGuard hasInclGuard(string const& content);

bool isUuidInclGuard(string const& inclGuard);

string generateInclGuard();

fs::path makePathFromString(string fileName);

typedef vector<fs::path> Paths;
typedef Paths::const_iterator PathConstIterator;
Paths makePathsFromStrings(vector<string> const& fileNames);

bool isReadWriteFile(fs::file_status const& fstat);

bool isReadWriteFile(fs::path const& path);

bool canReadWriteFiles(Paths const& paths);

bool isCppSourceFile(fs::path const& path);

bool areCppSourceFiles(Paths const& paths);

#endif
