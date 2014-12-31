#include "utils.hpp"
#include <boost/algorithm/string/predicate.hpp>
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <string>
#include <vector>
#include <algorithm>

TEST_CASE("search for generated copyright", "hasCopyrightNotice")
{
  std::string const company = "company XzR Ltd. AG & CoKG mbH";
  CHECK(hasCopyrightNotice(std::string("/*\n") +
			   " * Copyright 1997 " +
			   company +
			   " and Licensors. All Rights Reserved. Company Confidential.\n"
			   " */\n",
			   company));
  CHECK(hasCopyrightNotice(std::string("/*\n") +
			   " * Copyright 2013-2015 " +
			   company +
			   " and Licensors. All Rights Reserved. Company Confidential.\n"
			   " */\n",
			   company));
  CHECK_FALSE(hasCopyrightNotice(" * Copyright 1997 blah, blah, blah", company));
  CHECK_FALSE(hasCopyrightNotice(" * Copyright 2013-2015 blah, blah, blah", company));
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
      CHECK(g3 != g1);
      CHECK(g3 != g2);
      std::string const g4 = generateInclGuard();
      CHECK(g4 != g1);
      CHECK(g4 != g2);
      CHECK(g4 != g3);
      std::string const g5 = generateInclGuard();
      CHECK(g5 != g1);
      CHECK(g5 != g2);
      CHECK(g5 != g3);
      CHECK(g5 != g4);
    }
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

TEST_CASE("is header file shall return true if given path points to a header file otherwise false", "isHeaderFile")
{
  using boost::filesystem::path;
  CHECK(isHeaderFile(path("this/is/a/header/file.h")));
  CHECK(isHeaderFile(path("this/is/a/header/file.hpp")));
}

// TEST_CASE("partition by read write cpp files shall put all directories to front of the paths container", "partitionByReadWriteCppFile")
// {
//   REQUIRE(false);
// }

// TEST_CASE("add cpp files shall add cpp/header files from given directory paths", "addCppFilesFromDirectory")
// {
//   REQUIRE(false);
// }
