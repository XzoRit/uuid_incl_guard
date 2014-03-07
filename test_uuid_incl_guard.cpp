#include "utils.hpp"
#define CATCH_CONFIG_MAIN
#include <catch.hpp>

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
