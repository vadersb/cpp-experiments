//
// Created by Alexander Samarin on 17.07.2021.
//

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "test_func.h"

TEST_CASE("A test test :)")
{
	REQUIRE( 2 + 2 == 4 );
	REQUIRE( st::TestFunc(123) == 249 );
}