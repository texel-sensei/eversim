#include "core/reflection/dynamic_member.h"
#include "core/reflection/type.h"

#include <catch.hpp>
#include <string>

using namespace std::literals;
using namespace eversim::core::reflection;

namespace {
	struct teststruct
	{
		int a;
		int b;
		std::string name;
		void* some_data;
		char some_other_data[16];
	};
}

BOOST_FUSION_ADAPT_STRUCT(
	teststruct,
	a,b, name, some_data, some_other_data
)

TEST_CASE("reflection get_member_by_index", "[utility][reflection]")
{
	teststruct t1;
	t1.a = 42;
	t1.name = "Name!";
	t1.some_data = &t1;
	
	SECTION("Simple member"){
		const auto mem = get_member_by_index<0>(t1);
		REQUIRE(mem.name == "a"s);
		REQUIRE(mem.value == 42);
		REQUIRE(mem.type == type_c<int>);
	}

	SECTION("Non trivial member")
	{
		const auto mem = get_member_by_index<2>(t1);
		REQUIRE(mem.name == "name"s);
		REQUIRE(mem.value == "Name!");
		REQUIRE(mem.type == type_c<std::string>);
	}

	SECTION("pointer member")
	{
		const auto mem = get_member_by_index<3>(t1);
		REQUIRE(mem.name == "some_data"s);
		REQUIRE(mem.value == &t1);
		REQUIRE(mem.type == type_c<void*>);
	}

	SECTION("array member")
	{
		const auto mem = get_member_by_index<4>(t1);
		REQUIRE(mem.name == "some_other_data"s);
		REQUIRE(mem.type == type_c<char[16]>);
	}
}

TEST_CASE("Member map", "[utility][reflection")
{
	const auto mm = get_member_map<teststruct>();
	const auto member = mm.at("some_other_data");

	REQUIRE(member.name == "some_other_data"s);
	REQUIRE(member.byte_offset == offsetof(teststruct, some_other_data));
	REQUIRE(member.byte_size == sizeof(teststruct::some_other_data));

	teststruct t1;
	auto* adr = reinterpret_cast<char*>(&t1) + member.byte_offset;
	std::fill(adr, adr + member.byte_size, char(42));
	for(auto c : t1.some_other_data)
	{
		REQUIRE(c == 42);
	}
}