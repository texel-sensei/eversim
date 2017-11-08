#include "test_helper.h"

#include "core/world/level.h"
#include "core/world/tile_descriptor.h"

#include <catch.hpp>

using namespace eversim::core::world;

namespace {
	struct level_testing_data {
	protected:
		tile_descriptor tA{ "A" }, tB{ "B" }, tC{ "C" }, tD{ "D" };

		tile_descriptor const* A = &tA, *B = &tB, *C = &tC, *D = &tD;
		tile_descriptor const* O = nullptr;

		tile_descriptor const* desc_at(level const& l, int x, int y) const
		{
			return l.get_tile_by_index({ x,y }).get_descriptor();
		}

		std::string const& name_at(level const& l, int x, int y) const
		{
			return desc_at(l, x, y)->name;
		}
	};
}

TEST_CASE("default tiles empty", "[world][level]")
{
	auto l = level({ 5,5 });
	for (auto x = 0; x < l.get_num_tiles().x; ++x)
	{
		for (auto y = 0; y < l.get_num_tiles().y; ++y)
		{
			auto& t = l.get_tile_by_index({ x,y });
			REQUIRE(t.get_descriptor() == &blank_tile);
		}
	}
}


TEST_CASE_METHOD(level_testing_data,"layout", "[world][level]")
{
	tile_descriptor const* level_data[] = {
	//	0 1 2 3 4
		A,O,D,O,O,// 2
		A,C,O,O,A,// 1
		O,B,O,O,O // 0
	//	0 1 2 3 4
	};

	auto l = level({ 5,3 }, level_data);

	REQUIRE(name_at(l, 0, 0) == blank_tile.name);
	REQUIRE(name_at(l, 1, 0) == B->name);
	REQUIRE(name_at(l, 0, 1) == A->name);
	REQUIRE(name_at(l, 1, 1) == C->name);
	REQUIRE(name_at(l, 2, 2) == D->name);
	REQUIRE(name_at(l, 4, 1) == A->name);
}

TEST_CASE_METHOD(level_testing_data, "data constructor", "[world][level]")
{
	tile_descriptor test;
	auto const* X = &test;

	tile_descriptor const* level_data[] = {
	//	0 1 2 3 4 5
		X,O,X,O,O,O, // 6
		O,O,X,O,O,O, // 5
		O,O,X,O,O,O, // 4
		O,O,O,X,X,X, // 3
		X,X,X,O,O,O, // 2
		X,X,X,O,O,O, // 1
		X,X,X,O,O,O  // 0
	//  0 1 2 3 4 5
	};

	

	SECTION("too few data")
	{
		REQUIRE_THROWS(level({999,999}, level_data));
	}

	SECTION("correct")
	{
		const auto l = level({6,7}, level_data);

		SECTION("size")
		{
			REQUIRE(l.get_width() == 6);
			REQUIRE(l.get_height() == 7);
			REQUIRE(l.get_num_tiles().x == l.get_width());
			REQUIRE(l.get_num_tiles().y == l.get_height());
		}

		REQUIRE(desc_at(l,1,5) == &blank_tile);

		REQUIRE(desc_at(l,0,6) == X);

		REQUIRE(desc_at(l,2,6) == X);
		REQUIRE(desc_at(l,2,5) == X);
		REQUIRE(desc_at(l,2,4) == X);

		REQUIRE(desc_at(l,3,3) == X);
		REQUIRE(desc_at(l,4,3) == X);
		REQUIRE(desc_at(l,5,3) == X);

		REQUIRE(desc_at(l,0,0) == X);
		REQUIRE(desc_at(l,1,1) == X);
		REQUIRE(desc_at(l,2,2) == X);
	}
}

TEST_CASE_METHOD(level_testing_data,"tile collision ids", "[world][tile]")
{
	tile_descriptor solid{"X"};
	solid.collision = collision_type::solid;

	auto const* X = &solid;

	tile_descriptor const* level_data[] = {
	//	0 1 2 3 4 5
		X,O,X,O,O,O, // 6
		O,O,X,O,O,O, // 5
		O,O,X,O,O,O, // 4
		O,O,O,X,X,X, // 3
		X,X,X,O,O,O, // 2
		X,X,X,O,O,O, // 1
		X,X,X,O,O,O  // 0
	//  0 1 2 3 4 5
	};

	// collision shape is left,down,right,up

	auto l = level({ 6,7 }, level_data);

	const auto shape = [&l](int x, int y)
	{
		return l.get_tile_by_index({ x,y }).calculate_collision_shape();
	};

	REQUIRE(shape(0,6) == 0b0000);

	REQUIRE(shape(2,6) == 0b0100);
	REQUIRE(shape(2,5) == 0b0101);
	REQUIRE(shape(2,4) == 0b0001);

	REQUIRE(shape(3, 3) == 0b0010);
	REQUIRE(shape(4, 3) == 0b1010);
	REQUIRE(shape(5, 3) == 0b1000);

	REQUIRE(shape(1, 1) == 0b1111);
}

TEST_CASE("tile size", "[world][level]")
{
	auto l = level({ 10,1 });
	REQUIRE(l.get_num_tiles().x == 10);
	REQUIRE(l.get_num_tiles().y == 1);

	l.set_tile_size(0.75f);
	REQUIRE(l.get_tile_size() == Approx(0.75f));
	REQUIRE(l.get_world_size().x == Approx(7.5f));
	REQUIRE(l.get_world_size().y == Approx(0.75f));

	auto& t = l.get_tile_by_index({ 1,0 });
	REQUIRE(t.size() == Approx(0.75f));

	SECTION("inside/outside test")
	{
		const auto p = glm::vec2{ 1,.5f };
		auto& tile = l.get_tile_by_pos(p);
		REQUIRE(tile.point_inside(p));
		REQUIRE(!tile.point_inside(p + glm::vec2(3, 3)));
		REQUIRE(!tile.point_inside(p - glm::vec2(3, 3)));
	}
}

TEST_CASE("contains index", "[world][level]")
{
	auto l = level({ 3,3 });
	REQUIRE(l.contains_index({ 0,0 }));
	REQUIRE(l.contains_index({ 1,1 }));
	REQUIRE(l.contains_index({ 2,0 }));
	REQUIRE(l.contains_index({ 0,2 }));
	REQUIRE(l.contains_index({ 2,2 }));

	REQUIRE(!l.contains_index({ -1,-1 }));
	REQUIRE(!l.contains_index({ 0,3 }));
	REQUIRE(!l.contains_index({ 3,0 }));
	REQUIRE(!l.contains_index({ 3,3 }));
}

TEST_CASE("index / position conversion", "[world][level]")
{
	auto l = level({ 12,17 });
	
	l.set_tile_size(2.f);
	auto idx = l.index_for_pos({1.5f, 2.5f});
	REQUIRE(idx.x == 0);
	REQUIRE(idx.y == 1);
	auto pos = l.center_of_tile({0,1});
	REQUIRE(pos.x == Approx(1.f));
	REQUIRE(pos.y == Approx(3.f));

	auto& t = l.get_tile_by_pos(pos);
	REQUIRE(t.x() == Approx(pos.x));
	REQUIRE(t.y() == Approx(pos.y));
}

TEST_CASE_METHOD(level_testing_data,"tile neighbours", "[world][tile]")
{
	tile_descriptor const* level_data[] = {
		O,B,O,
		A,A,C,
		O,D,O
	};

	auto l = level({ 3,3 }, level_data);
	auto const& t = l.get_tile_by_index({ 1,1 });

	SECTION("valid neighbours"){
		auto const* n = t.get_neighbour({ 1,0 });
		REQUIRE(n->index() == glm::ivec2(2, 1));
		REQUIRE(n->get_neighbour({ -1,0 }) == &t);

		REQUIRE(t.get_neighbour({ 1,0 })->get_descriptor() == C);
		REQUIRE(t.get_neighbour({-1,0 })->get_descriptor() == A);
		REQUIRE(t.get_neighbour({ 0,1 })->get_descriptor() == B);
		REQUIRE(t.get_neighbour({ 0,-1})->get_descriptor() == D);
		REQUIRE(t.get_neighbour({ 1,1 })->get_descriptor() == &blank_tile);
	}

	SECTION("invalid neighbours")
	{
		auto const* n = t.get_neighbour({ 2,0 });
		REQUIRE(n == nullptr);
	}
}

TEST_CASE("tile positions", "[world][level]")
{
	auto l = level({ 5,5 });
	l.set_tile_size(1.15f);

	for(auto x = 0; x < l.get_num_tiles().x; ++x)
	{
		for (auto y = 0; y < l.get_num_tiles().y; ++y)
		{
			auto& t = l.get_tile_by_index({ x,y });
			auto center = l.center_of_tile({ x,y });

			REQUIRE(t.index().x == x);
			REQUIRE(t.index().y == y);

			REQUIRE(t.x() == Approx(center.x));
			REQUIRE(t.y() == Approx(center.y));
		}
	}
}

TEST_CASE("level collision shape building", "[world][level]")
{
	auto l = level({ 3,3 });

	for(unsigned char c = 0; c < 16; ++c)
	{
		const auto v = l.get_collision_shape(c);
		const auto bits = std::bitset<4>(c);
		const auto zeros = bits.size() - bits.count();
		REQUIRE(v.size() == zeros);
	}
}

TEST_CASE("tile: world to tile coordinates", "[world][tile]")
{
	using namespace glm;
	auto l = level({ 3,3 });
	l.set_tile_size(2.5);
	auto const& t = l.get_tile_by_index({ 1,1 });

	const auto center = t.position();
	REQUIRE_THAT(t.to_tile_coordinates(center), is_approx(0, 0));
	REQUIRE_THAT(t.to_tile_coordinates(center + vec2(0.125,0)), is_approx(0.1, 0));
	REQUIRE_THAT(t.to_tile_coordinates(center - vec2(0.125, 0)), is_approx(-0.1, 0));
	
	const auto upper_left = center + vec2(-1.249, 1.249);
	REQUIRE(&l.get_tile_by_pos(upper_left) == &t);
	REQUIRE_THAT(t.to_tile_coordinates(upper_left), is_approx(-0.9992f, 0.9992f));
}