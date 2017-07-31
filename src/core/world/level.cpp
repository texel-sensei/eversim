#include "core/world/level.h"
#include "core/world/tile.h"
#include <glm/detail/type_mat.hpp>

using namespace boost;
using namespace glm;

namespace eversim { namespace core { namespace world {

	level::level(ivec2 size)
		: tiles(extents[size.x][size.y])
	{
		for (auto x = 0; x < get_num_tiles().x; ++x)
		{
			for (auto y = 0; y < get_num_tiles().y; ++y)
			{
				auto& t = get_tile_by_index({ x,y });
				t = tile{ {x,y}, tile_size };
				t.lvl = this;
			}
		}
	}

	void level::initialize_graphics(rendering::render_manager& mng)
	{
		for (auto x = 0; x < get_num_tiles().x; ++x)
		{
			for (auto y = 0; y < get_num_tiles().y; ++y)
			{
				auto& t = get_tile_by_index({ x,y });
				t.initialize_graphics(mng);
			}
		}
	}

	void level::set_tile_size(float f)
	{
		tile_size = f;
		for(auto i = 0u; i < tiles.num_elements(); ++i)
		{
			auto& t = tiles.data()[i];
			t.size(tile_size);
		}
	}

	vec2 level::get_world_size() const
	{
		return vec2(get_num_tiles()) * tile_size;
	}

	ivec2 level::get_num_tiles() const
	{
		return {tiles.shape()[0], tiles.shape()[1]};
	}

	tile& level::get_tile_by_index(ivec2 idx)
	{
		return tiles[idx.x][idx.y];
	}

	tile& level::get_tile_by_pos(vec2 pos)
	{
		const auto index = index_for_pos(pos);
		return get_tile_by_index(index);
	}

	tile const& level::get_tile_by_index(ivec2 idx) const
	{
		return tiles[idx.x][idx.y];
	}

	tile const& level::get_tile_by_pos(vec2 pos) const
	{
		const auto index = index_for_pos(pos);
		return get_tile_by_index(index);
	}

	ivec2 level::index_for_pos(vec2 p) const
	{
		return {p / tile_size};
	}

	vec2 level::center_of_tile(ivec2 idx) const
	{
		return tile_size * vec2(idx) + vec2(tile_size / 2.f);
	}

	bool level::contains_index(ivec2 idx) const noexcept
	{
		if (idx.x < 0 || idx.y < 0) return false;
		if (idx.x >= tiles.shape()[0] || idx.y >= tiles.shape()[1]) return false;
		return true;
	}

}}}
