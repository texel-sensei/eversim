#include "core/utility/helper.h"
#include <catch.hpp>

using namespace eversim::core::utility;

TEST_CASE( "final_action calls function", "[utility][helper.h]")
{
	bool was_called = false;
	auto to_call = [&] {was_called = true; };
	{
		auto _ = finally(to_call);	
		REQUIRE(was_called == false);
	}
	REQUIRE(was_called == true);
}