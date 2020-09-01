#include "core/utility/spatial_hashmap.h"
#include "core/utility/helper.h"
#include <catch2/catch.hpp>
#include <iostream>

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

	test_map.clear();
	REQUIRE(test_map.empty());
	REQUIRE(test_map.size() == 0);
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
			auto cell = test_map.get_cell(vec2{});
			auto size = std::distance(cell.begin(), cell.end());
			REQUIRE(size == 1);

			for (auto& i : test_map.get_cell(vec2{}))
			{
				REQUIRE(i == 1);
			}
		}

		SECTION("multiple in same cell")
		{
			test_map.insert({}, 2);
			auto cell = test_map.get_cell(vec2{});

			auto it = cell.begin();
			REQUIRE(*it == 2);
			++it;
			REQUIRE(*it == 1);
			++it;
			REQUIRE(it == cell.end());

			auto expected = { 2,1 };
			REQUIRE(std::equal(cell.begin(), cell.end(), expected.begin(), expected.end()));
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

namespace {
	struct mycmp {
		bool operator()(ivec2 a, ivec2 b) const {
			return std::tie(a.x, a.y) < std::tie(b.x, b.y);
		}
	};
}

TEST_CASE("spatial hashmap resize", "[utility][spatial_hashmap]")
{
	spatial_hashmap<int, std::atomic<int>> the_map(1.f, 5, 10);
	the_map.reset(200, 3);
	std::set<int> numbers;
	std::set<ivec2, mycmp> positions;
	for(int i = 0; i < 100; ++i)
	{
		const auto pos = vec2(i/10.f, i*i/10.f);
		const auto idx = the_map.get_index(pos);
		the_map.insert(idx,i);
		numbers.insert(i);
		positions.insert(idx);
	}

	for(auto idx : positions)
	{
		for(auto i : the_map.get_cell(idx))
		{
			numbers.erase(i);
		}
	}
	REQUIRE(numbers.empty());
}
