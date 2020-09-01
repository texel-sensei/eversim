
#include "core/utility/event_delegate.h"
#include <catch2/catch.hpp>

using namespace eversim::core::utility;

TEST_CASE("event void dispatch", "[utility][event_delegate]") {
	int num_calls_a = 0;
	int num_calls_b = 0;
	auto must_be_called_a = [&num_calls_a]{num_calls_a++;};
	auto must_be_called_b = [&num_calls_b]{num_calls_b++;};
	Event<> e;

	Delegate a;
	a.connect(must_be_called_a, e);
	SECTION("single delegate"){
		e();
		REQUIRE(num_calls_a == 1);
		REQUIRE(num_calls_b == 0);
	}
	SECTION("multiple delegates"){
		Delegate b;
		b.connect(must_be_called_b, e);

		e();
		e();
		REQUIRE(num_calls_a == 2);
		REQUIRE(num_calls_b == 2);
	}
	SECTION("insert & remove")
	{
		e();
		{
			Delegate b;
			b.connect(must_be_called_b, e);

			e();
			REQUIRE(num_calls_a == 2);
			REQUIRE(num_calls_b == 1);
		}
		e();
		REQUIRE(num_calls_a == 3);
		REQUIRE(num_calls_b == 1);
	}
}

TEST_CASE("event argument dispatch", "[utility][event_delegate]") {
	bool called = false;
	auto must_be_called = [&called](int n, std::string str){
		REQUIRE(n == 42);
		REQUIRE(str == "Hello World!");
		called = true;
	};
	Event<int, std::string> e;
	Delegate a;
	a.connect(must_be_called, e);

	e(42,"Hello World!");

	REQUIRE(called == true);
}
