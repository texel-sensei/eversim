#include "core/utility/area.h"
#include <catch.hpp>
#include <iostream>
using namespace eversim::core::utility;

TEST_CASE("quad_fill","[utility][areadivider]") {

	Areadivider a({128,128});

	auto pos = a.place_rectangle({64,64});
	REQUIRE(pos == glm::ivec2(0,0));

	pos = a.place_rectangle({ 64,64 });
	REQUIRE(pos != glm::ivec2(-1, -1));

	pos = a.place_rectangle({ 64,64 });
	REQUIRE(pos != glm::ivec2(-1, -1));

	pos = a.place_rectangle({ 64,64 });
	REQUIRE(pos != glm::ivec2(-1, -1));

	pos = a.place_rectangle({ 64,64 });
	REQUIRE(pos == glm::ivec2(-1, -1));

}

TEST_CASE("irregular_quad_fill","[utility][areadivider]")
{
	Areadivider a({ 128,128 });

	//fill with three 64 cubes
	auto pos = a.place_rectangle({ 64,64 });
	REQUIRE(pos == glm::ivec2(0, 0));

	pos = a.place_rectangle({ 64,64 });
	REQUIRE(pos != glm::ivec2(-1, -1));

	pos = a.place_rectangle({ 64,64 });
	REQUIRE(pos != glm::ivec2(-1, -1));

	//now theres a 64 square empty, fill it with three 32 cubes
	pos = a.place_rectangle({ 32,32 });
	REQUIRE(pos != glm::ivec2(-1, -1));

	pos = a.place_rectangle({ 32,32 });
	REQUIRE(pos != glm::ivec2(-1, -1));

	pos = a.place_rectangle({ 32,32 });
	REQUIRE(pos != glm::ivec2(-1, -1));

	//same thing, but fill everything
	pos = a.place_rectangle({ 16,16 });
	REQUIRE(pos != glm::ivec2(-1, -1));

	pos = a.place_rectangle({ 16,16 });
	REQUIRE(pos != glm::ivec2(-1, -1));

	pos = a.place_rectangle({ 16,16 });
	REQUIRE(pos != glm::ivec2(-1, -1));

	pos = a.place_rectangle({ 16,16 });
	REQUIRE(pos != glm::ivec2(-1, -1));

	//should be empty now
	pos = a.place_rectangle({ 16,16 });
	REQUIRE(pos == glm::ivec2(-1, -1));
}