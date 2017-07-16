#include "core/utility/spatial_hashmap.h"
#include <catch.hpp>

using namespace glm;
using namespace eversim::core::utility;

TEST_CASE("size_test", "[utility][spatial_hashmap]")
{
	spatial_hashmap<int> test_map;
	REQUIRE(test_map.empty());
	REQUIRE(test_map.size() == 0);

	const auto pos = vec2{};
	test_map.insert(pos, 7);
	REQUIRE(!test_map.empty());
	REQUIRE(test_map.size() == 1);

	int i = 7;
	test_map.insert(pos, i);
	REQUIRE(!test_map.empty());
	REQUIRE(test_map.size() == 2);
}

TEST_CASE("iteration", "[utility][spatial_hashmap]")
{
	spatial_hashmap<int> test_map;
	SECTION("iterate empty") {
		REQUIRE(test_map.empty());
		for (auto& i : test_map.get_cell(vec2{}))
		{
			FAIL("Found value in empty hashmap! i = " << i);
		}
	}

	SECTION("with elements") {
		test_map.insert({}, 1);
		SECTION("single element") {
			for(auto& i : test_map.get_cell(vec2{}))
			{
				REQUIRE(i == 1);
			}
			auto cell = test_map.get_cell(vec2{});
			auto size = std::distance(cell.begin(), cell.end());
			REQUIRE(size == 1);
		}

		SECTION("multiple in same cell")
		{
			test_map.insert({}, 2);
			auto cell = test_map.get_cell(vec2{});
			auto expected = { 1,2 };
			REQUIRE(equal(cell.begin(), cell.end(), expected.begin(), expected.end()));
		}
		SECTION("multiple elements in different cells")
		{
			test_map.insert({0.f, test_map.get_cell_size()}, 2);
			REQUIRE(test_map.size() == 2);
			auto cell = test_map.get_cell(vec2{});
			auto size = std::distance(cell.begin(), cell.end());
			REQUIRE(size == 1);
			REQUIRE(*cell.begin() == 1);
		}
	}
}
