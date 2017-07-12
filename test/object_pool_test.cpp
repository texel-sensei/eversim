#include "core/utility/object_pool.h"
#include <catch.hpp>

#include <algorithm>
#include <vector>
#include <numeric>

using namespace std;
using namespace eversim::core::utility;

TEST_CASE("object_pool c'tor/d'tor", "[utility][object_pool]")
{
	struct tester {
		int* v;
		tester(int* p) : v(p) { *v = *v + 1; }
		~tester() {*v = *v -1; }
		tester(tester const&) = delete;
		tester& operator=(tester const&) = delete;
	};

	int check = 0;
	object_pool<tester, 2> the_pool;

	SECTION("single object test") {
		REQUIRE(check == 0);
		auto obj_ptr = the_pool.emplace(&check);
		REQUIRE(check == 1);
		const auto it = the_pool.locate(obj_ptr);
		REQUIRE(it != the_pool.end());
		the_pool.erase(it);
		REQUIRE(check == 0);
	}

	SECTION("multiple objects")
	{
		REQUIRE(check == 0);
		for(int i = 0; i < 30; ++i)
		{
			the_pool.emplace(&check);
			REQUIRE(check == (i+1));
		}
		REQUIRE(check == 30);

		SECTION("deletion")
		{
			while(!the_pool.empty())
			{
				the_pool.erase(the_pool.begin());
			}
			REQUIRE(check == 0);
		}
	}
}

TEST_CASE("object_pool locate", "[utility][object_pool]")
{
	object_pool<long long,4> the_pool;
	long long* seven_ptr = nullptr;
	for(int i = 0; i < 20; ++i)
	{
		auto ptr = the_pool.insert(i);
		if (i == 7)
			seven_ptr = ptr;
	}

	REQUIRE(*seven_ptr == 7);

	SECTION("locate invalid")
	{
		REQUIRE(the_pool.locate(nullptr) == the_pool.end());
		long long something_else = 9;
		auto other_ptr = &something_else;
		REQUIRE(the_pool.locate(other_ptr) == the_pool.end());
	}

	SECTION("find and locate simple") {
		auto find_it = find(the_pool.begin(), the_pool.end(), 7);
		REQUIRE(*find_it == 7);

		auto locate_it = the_pool.locate(seven_ptr);
		REQUIRE(locate_it != the_pool.end());
		REQUIRE(find_it == locate_it);
		REQUIRE(*locate_it == 7);
	}
	SECTION("find and locate extra")
	{
		for(auto& obj : the_pool)
		{
			auto find_it = find(the_pool.begin(), the_pool.end(), obj);
			REQUIRE(*find_it == obj);
			REQUIRE(&(*find_it) == &obj);

			auto locate_it = the_pool.locate(&obj);
			INFO("Current object is " << obj);
			REQUIRE(find_it == locate_it);
			REQUIRE(*locate_it == obj);
		}
	}
}

TEST_CASE("object_pool size", "[utility][object_pool]")
{
	object_pool<long long> the_pool;
	REQUIRE(the_pool.size() == 0);
	REQUIRE(the_pool.empty() == true);

	the_pool.insert(5);

	REQUIRE(the_pool.size() == 1);
	REQUIRE(the_pool.empty() == false);

	the_pool.insert(7);

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
		the_pool.insert(5);
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
			the_pool.insert(i);
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