#include "utils.hpp"
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <string>

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
