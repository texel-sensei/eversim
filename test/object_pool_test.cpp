#include "core/utility/object_pool.h"
#include <catch.hpp>

#include <algorithm>
#include <vector>
#include <numeric>

using namespace std;
using namespace eversim::core::utility;

TEST_CASE("object_pool size", "[utility][object_pool]")
{
	object_pool<long long> the_pool;
	REQUIRE(the_pool.size() == 0);
	REQUIRE(the_pool.empty() == true);

	the_pool.push_back(5);

	REQUIRE(the_pool.size() == 1);
	REQUIRE(the_pool.empty() == false);

	the_pool.push_back(7);

	REQUIRE(the_pool.size() == 2);
}

TEST_CASE("Iterating object_pool", "[utility][object_pool]")
{
	object_pool<long long, 3> the_pool;
	SECTION("Iterate empty")
	{
		REQUIRE(the_pool.empty());
		for(auto&& i : the_pool)
		{
			FAIL("Found objects in an empty pool! Found: " << i );
		}
	}

	
	SECTION("Iterate single element")
	{
		the_pool.push_back(5);
		REQUIRE(the_pool.size() == 1);
		for(auto&& e : the_pool)
		{
			REQUIRE(e == 5);
		}
	}
	SECTION("iterate many objects")
	{
		auto numbers = vector<int>(30, 0);
		for(int i = 0; i < numbers.size(); ++i)
		{
			the_pool.push_back(i);
		}

		SECTION("find all elements")
		{
			for(auto i : the_pool)
			{
				REQUIRE(i < (int)numbers.size());
				REQUIRE(numbers[i] == 0);
				numbers[i] = 1;
			}
			REQUIRE((size_t)count(numbers.begin(), numbers.end(),1) == numbers.size());
		}
		SECTION("standard algorithm")
		{
			fill(the_pool.begin(), the_pool.end(), 7);
			auto equals_7 = [](auto i) {return i == 7; };
			REQUIRE(all_of(the_pool.begin(), the_pool.end(), equals_7));
		}
		SECTION("With erased objects")
		{
			auto is_odd = [](auto i) {return i % 2 == 1; };
			iota(numbers.begin(), numbers.end(),0);
			numbers.erase(
				remove_if(numbers.begin(), numbers.end(), is_odd),
				numbers.end()
			);

			auto old_size = the_pool.size();

			the_pool.erase(
				remove_if(the_pool.begin(), the_pool.end(), is_odd),
				the_pool.end()
			);

			REQUIRE(the_pool.size() < old_size);
			REQUIRE(the_pool.size() == numbers.size());

			SECTION("only erased")
			{
				REQUIRE(none_of(the_pool.begin(), the_pool.end(), is_odd));
			}
			SECTION("after inserts")
			{
				the_pool.insert(numbers.begin(), numbers.end());
				REQUIRE(none_of(the_pool.begin(), the_pool.end(), is_odd));
			}
		}
	}
}