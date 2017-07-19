#include "core/utility/object_pool.h"
#include <catch.hpp>

#include <algorithm>
#include <vector>
#include <numeric>
#include <random>

using namespace std;
using namespace eversim::core::utility;

TEST_CASE("object_pool c'tor/d'tor", "[utility][object_pool]")
{
	struct tester {
		int* v;
		tester(int* p) : v(p) { *v = *v + 1; }
		~tester() { *v = *v - 1; }
		tester(tester const&) = delete;
		tester& operator=(tester const&) = delete;
	};

	int check = 0;
	object_pool<tester, 2> the_pool;

	SECTION("single object test")
	{
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
		for (int i = 0; i < 30; ++i)
		{
			the_pool.emplace(&check);
			REQUIRE(check == (i+1));
		}
		REQUIRE(check == 30);

		SECTION("deletion")
		{
			while (!the_pool.empty())
			{
				the_pool.erase(the_pool.begin());
			}
			REQUIRE(check == 0);
		}
	}
}

TEST_CASE("object_pool locate", "[utility][object_pool]")
{
	object_pool<long long, 4> the_pool;
	long long* seven_ptr = nullptr;
	for (int i = 0; i < 20; ++i)
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

	SECTION("find and locate simple")
	{
		auto find_it = find(the_pool.begin(), the_pool.end(), 7);
		REQUIRE(*find_it == 7);

		auto locate_it = the_pool.locate(seven_ptr);
		REQUIRE(locate_it != the_pool.end());
		REQUIRE(find_it == locate_it);
		REQUIRE(*locate_it == 7);
	}
	SECTION("find and locate extra")
	{
		for (auto& obj : the_pool)
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
		for (auto&& i : the_pool)
		{
			FAIL("Found objects in an empty pool! Found: " << i );
		}
	}


	SECTION("Iterate single element")
	{
		the_pool.insert(5);
		REQUIRE(the_pool.size() == 1);
		for (auto&& e : the_pool)
		{
			REQUIRE(e == 5);
		}

		SECTION("iterate const")
		{
			auto const& cpool = the_pool;
			REQUIRE(cpool.size() == 1);
			for (auto&& e : cpool)
			{
				REQUIRE(e == 5);
			}
		}
	}
	SECTION("iterate many objects")
	{
		auto numbers = vector<int>(30, 0);
		for (int i = 0; i < numbers.size(); ++i)
		{
			the_pool.insert(i);
		}

		SECTION("find all elements")
		{
			for (auto i : the_pool)
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
			auto equals_7 = [](auto i) { return i == 7; };
			REQUIRE(all_of(the_pool.begin(), the_pool.end(), equals_7));
		}
		SECTION("With erased objects")
		{
			auto is_odd = [](auto i) { return i % 2 == 1; };
			iota(numbers.begin(), numbers.end(), 0);
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

TEST_CASE("object_pool memory reuse", "[utility][object_pool]")
{
	object_pool<long long, 4> the_pool;

	auto* mem = the_pool.insert(7);
	REQUIRE(the_pool.size() == 1);
	the_pool.erase(the_pool.locate(mem));
	REQUIRE(the_pool.size() == 0);
	auto* new_mem = the_pool.insert(7);
	REQUIRE(the_pool.size() == 1);
	REQUIRE(mem == new_mem);
}

TEST_CASE("object_pool insert/erase", "[utility][object_pool]")
{
	struct gadget {
		char stuff[75];

		gadget(size_t s = 0)
		{
			*reinterpret_cast<size_t*>(stuff) = s;
		}

		bool operator==(gadget const& other) const
		{
			auto b1 = begin(stuff);
			auto b2 = begin(other.stuff);
			return equal(b1, b1 + sizeof(size_t), b2, b2 + sizeof(size_t));
		}
	};

	object_pool<gadget, 11> the_pool;

	const int N = 100;
	map<gadget*, gadget> objects;

	mt19937 rng;
	auto num_dist = uniform_int_distribution<long long>();
	auto coinflip = bernoulli_distribution(0.3); // 30% chance to remove an object
	for (int i = 0; i < N; ++i)
	{
		if (coinflip(rng) && !the_pool.empty())
		{
			auto d = uniform_int_distribution<>(0, the_pool.size() - 1);
			auto it = the_pool.begin();
			advance(it, d(rng));
			auto* obj = &*it;
			it = the_pool.locate(obj);
			auto map_it = objects.find(obj);
			REQUIRE(map_it != objects.end());
			REQUIRE(map_it->second == *obj);

			auto oldsize = the_pool.size();
			the_pool.erase(it);
			REQUIRE(the_pool.size() == oldsize -1);
		} else
		{
			auto oldsize = the_pool.size();
			auto ptr = the_pool.insert(num_dist(rng));
			objects[ptr] = *ptr;
			REQUIRE(the_pool.size() == oldsize + 1);
		}
	}
	for (auto& o : the_pool)
	{
		auto map_it = objects.find(&o);
		REQUIRE(map_it != objects.end());
		REQUIRE(map_it->second == o);
	}
	for (auto& p : objects)
	{
		auto pool_it = the_pool.locate(p.first);
		REQUIRE(pool_it != the_pool.end());
		REQUIRE(*pool_it == p.second);
	}

	generate(the_pool.begin(), the_pool.end(), ref(rng));

	while(!the_pool.empty())
	{
		auto d = uniform_int_distribution<>(0, the_pool.size() - 1);
		auto it = the_pool.begin();
		advance(it, d(rng));
		the_pool.erase(it);
	}
	REQUIRE(the_pool.empty());
}
