#include "core/utility/time/controllable_clock.h"
#include "core/utility/time/scoped_timer.h"
#include <catch.hpp>

using namespace eversim::core::utility;

using namespace std;
using namespace chrono_literals;

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

TEST_CASE("scoped timer", "[utility][time][scoped_timer]")
{
	auto clk = make_shared<controllable_clock>();
	auto test_short = [](clock::duration dur) { REQUIRE(dur == 2s); };
	auto test_long = [](clock::duration dur) { REQUIRE(dur == 12s); };
	{
		scoped_timer long_timer(clk, test_long);
		clk->advance(5s);
		{
			scoped_timer short_timer(clk, test_short);
			clk->advance(2s);
		}
		clk->advance(5s);
	}
}