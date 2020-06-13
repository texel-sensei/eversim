#include "core/utility/array_view.h"
#include <catch.hpp>

using namespace std;
using namespace eversim::core::utility;

TEST_CASE("array_view creation","[utility][array_view]")
{
	SECTION("Default construct")
	{
		auto view = array_view<int>();
		REQUIRE(view.size() == 0);
		REQUIRE(view.data() == nullptr);
	}
	SECTION("From vector"){
		auto vec = vector<int>{1,2,3,4,5};

		auto view = make_array_view(vec);
		REQUIRE(view.data());
		REQUIRE(view.size() == vec.size());
		REQUIRE(view.data() == vec.data());
		static_assert(is_same_v<decltype(view)::element_type, int>, "Type inferrence broken!");
	}
	SECTION("From const vector") {
		const auto vec = vector<int>{ 1,2,3,4,5 };

		auto view = make_array_view(vec);
		REQUIRE(view.data());
		REQUIRE(view.size() == vec.size());
		REQUIRE(view.data() == vec.data());
		static_assert(is_same_v<decltype(view)::element_type, const int>, "Type inferrence broken!");
	}
	SECTION("From C-style array") {
		int data[] = { 1,2,3,4,5 };

		auto view = make_array_view(data);
		REQUIRE(view.data());
		REQUIRE(view.size() == sizeof(data) / sizeof(data[0]));
		REQUIRE(view.data() == data);
	}
}

namespace {
	template<typename T>
	bool operator==(array_view<T> const& a, vector<T> const& b) {
		return equal(a.begin(), a.end(), b.begin(), b.end());
	}
}

TEST_CASE("array view slice", "[utility][array_view]")
{
	int data[] = { 1,2,3,4,5 };
	auto view = make_array_view(data);
	SECTION("nop slice_n"){
		auto all = view.subspan(0, view.size());
		REQUIRE(equal(view.begin(), view.end(), all.begin(), all.end()));
	}
	SECTION("slice_n")
	{
		auto left = view.subspan(0, 3);
		auto expected_left = vector<int>{ 1,2,3 };
		REQUIRE(left.size() == 3);
		REQUIRE(left == expected_left);

		auto right = view.subspan(3, 2);
		auto expected_right = vector<int>{ 4,5 };
		REQUIRE(right.size() == 2);
		REQUIRE(right == expected_right);

		auto middle = view.subspan(1, 3);
		auto expected_middle = vector<int>{ 2,3,4 };
		REQUIRE(middle.size() == 3);
		REQUIRE(middle == expected_middle);
	}
}
