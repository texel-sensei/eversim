#include "core/utility/time/controllable_clock.h"
#include <catch.hpp>

using namespace eversim::core::utility;
using namespace std::chrono_literals;

TEST_CASE("controllable clock", "[utility][time][clock]")
{
	SECTION("default c'tor") {
		controllable_clock clk;
		REQUIRE(clk.now() == clock::time_point());
	}

	SECTION("time point c'tor")
	{
		auto clk = controllable_clock(clock::time_point(7h));
		REQUIRE(clk.now() == clock::time_point(7h));
	}

	SECTION("advance"){
		controllable_clock clk;
		clk.advance(5ms);
		REQUIRE(clk.now() == clock::time_point(5ms));

		const auto old_time = clk.now();
		clk.advance(7h);
		REQUIRE((clk.now() - old_time) == 7h);
	}

}