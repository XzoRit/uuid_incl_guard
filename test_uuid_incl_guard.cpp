#include "utils.hpp"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <string>
#include <vector>
#include <algorithm>

TEST_CASE("search for copyright notice is case insensitive", "hasCopyrightNotice")
{
  CHECK(hasCopyrightNotice("Copyright"));
  CHECK(hasCopyrightNotice("copyright"));
}

TEST_CASE("has include guard returns found guard", "hasInclGuard")
{
  MaybeInclGuard const guard = hasInclGuard("#ifndef _MY_TYPICAL_INCL_GUARD_H_");
  CHECK(guard.get() == "_MY_TYPICAL_INCL_GUARD_H_");
}

TEST_CASE("has include guard returns false if no guard could be found", "hasInclGuard")
{
  MaybeInclGuard const guard = hasInclGuard("");
  CHECK_FALSE(guard);
}

TEST_CASE("is uuid id include guard returns true if guard starts with "
	  "INCL "
	  "followed by _ followed by 8 hex digits "
	  "followed by _ followed by 4 hex digits "
	  "followed by _ followed by 4 hex digits "
	  "followed by _ followed by 4 hex digits "
	  "followed by _ followed by 8 hex digits ", "isUuidInclGuard")
{
  SECTION("lower case hex digits")
    {
      CHECK(isUuidInclGuard("INCL_01234567_89ab_cdef_ba98_76543210"));
    }
  SECTION("upper case hex digits")
    {
      CHECK(isUuidInclGuard("INCL_01234567_89AB_CDEF_BA98_76543210"));
    }
  SECTION("lower case incl prefix")
    {
      CHECK_FALSE(isUuidInclGuard("incl_01234567_89ab_cdef_ba98_76543210"));
    }
  SECTION("wrong group seperator")
    {
      CHECK_FALSE(isUuidInclGuard("INCL-01234567-89ab-cdef-ba98-76543210"));
    }
  SECTION("no incl prefix")
    {
      CHECK_FALSE(isUuidInclGuard("01234567_89ab_cdef_ba98_76543210"));
    }
}

TEST_CASE("generate incl guard shall return a random c/c++ conform guard symbol", "generateInclGuard")
{
  std::string const guard = generateInclGuard();

  SECTION("starts with INCL_")
    {
      using boost::algorithm::starts_with;
      CHECK(starts_with(guard, "INCL_"));
    }
  SECTION("contains 5 _-seperators")
    {
      int const seps = std::count(guard.begin(), guard.end(), '_');
      CHECK(seps == 5);
    }
  SECTION("uuids are random (sort of)")
    {
      std::string const g1 = generateInclGuard();
      std::string const g2 = generateInclGuard();
      CHECK(g1 != g2);
      std::string const g3 = generateInclGuard();
      std::string const g4 = generateInclGuard();
      CHECK(g3 != g4);
      std::string const g5 = generateInclGuard();
      std::string const g6 = generateInclGuard();
      CHECK(g5 != g6);
    }
}

TEST_CASE("make path from string converts a string into a path object", "makePathFromString")
{
  std::string const s = "a sample string for a path object";
  using boost::filesystem::path;
  path const p = makePathFromString(s);
  CHECK(p == s);  
}

TEST_CASE("make paths from strings converts a strings into a path objects", "makePathsFromStrings")
{
  std::vector<std::string> strings;
  strings.push_back("path_1");
  strings.push_back("path_2");
  strings.push_back("path_3");
  Paths const paths = makePathsFromStrings(strings);
  CHECK(std::equal(strings.begin(), strings.end(), paths.begin()));
}

TEST_CASE("is read write file returns true if file is readable and writeable", "isReadWriteFile")
{
  using namespace boost::filesystem;
  SECTION("is regular file, is readable by owner and is writeable")
    {
      file_status fs(regular_file, owner_read | owner_write);
      CHECK(isReadWriteFile(fs));
    }
  SECTION("file not found, is readable by owner and is writeable")
    {
      file_status fs(file_not_found, owner_read | owner_write);
      CHECK_FALSE(isReadWriteFile(fs));
    }
  SECTION("is directory file, is readable by owner and is writeable")
    {
      file_status fs(directory_file, owner_read | owner_write);
      CHECK_FALSE(isReadWriteFile(fs));
    }
  SECTION("is regular file, no permissionss")
    {
      file_status fs(directory_file, no_perms);
      CHECK_FALSE(isReadWriteFile(fs));
    }
}

TEST_CASE("is cpp source file returns true is path either ends with .c, .h, .cpp or .hpp", "isCppSourceFile")
{
  using namespace boost::filesystem;
  SECTION("ends with .c")
    {
      path const p = "this/is/a/test/path/to/a/c/source/file.c";
      CHECK(isCppSourceFile(p));
    }
  SECTION("ends with .h")
    {
      path const p = "this/is/a/test/path/to/a/c/header/file.h";
      CHECK(isCppSourceFile(p));
    }
  SECTION("ends with .cpp")
    {
      path const p = "this/is/a/test/path/to/a/cpp/source/file.cpp";
      CHECK(isCppSourceFile(p));
    }
  SECTION("ends with .hpp")
    {
      path const p = "this/is/a/test/path/to/a/cpp/header/file.hpp";
      CHECK(isCppSourceFile(p));
    }
}

TEST_CASE("are cpp source files returns true if all paths either ends with .c, .h, .cpp or .hpp", "areCppSourceFiles")
{
  Paths paths;
  paths.push_back("this/is/a/test/path/to/a/c/source/file.c");
  paths.push_back("this/is/a/test/path/to/a/c/header/file.h");
  paths.push_back("this/is/a/test/path/to/a/cpp/source/file.cpp");
  paths.push_back("this/is/a/test/path/to/a/cpp/header/file.hpp");
  CHECK(areCppSourceFiles(paths));
}
