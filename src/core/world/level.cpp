#include "core/world/level.h"
#include "core/world/tile.h"
#include <glm/detail/type_mat.hpp>
#include <easylogging++.h>

using namespace boost;
using namespace glm;

namespace eversim { namespace core { namespace world {

	level::level(ivec2 size, utility::array_view<tile_descriptor const*> data)
		: tiles(extents[size.y][size.x])
	{
		const auto num_tiles = size.x * size.y;

		const auto descriptors = multi_array_ref<tile_descriptor const*, 2>(
			data.data(), extents[size.y][size.x]
		);

		// pass null-view to initialize all tiles to blank
		// if we use a null-view we don't need to check size
		if(data){
			if(data.size() < num_tiles)
			{
				LOG(ERROR) << "Not enough data passed to level c'tor! Size is ("
					<< size.x << ", " << size.y << ") [" << num_tiles <<
					" tiles] but got only data for " << data.size() << " tiles!";
				throw std::runtime_error{"Too few data in level c'tor!"};
			}
			if(data.size() > num_tiles)
			{
				LOG(WARNING) << "Too much data passed to level c'tor! Size is ("
					<< size.x << ", " << size.y << ") [" << num_tiles <<
					" tiles] but got data for " << data.size() << " tiles!";
			}
		}

		for (auto y = 0; y < get_height(); ++y)
		{
			for (auto x = 0; x < get_width(); ++x)
			{
				auto& t = get_tile_by_index({ x,y });
				t = tile{ { x,y }, tile_size };
				t.lvl = this;
				if(data)
				{
					auto const* d = descriptors[size.y - y - 1][x];
					if (d){
						t.descriptor = d;
					}
				}
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
		return {get_width(), get_height()};
	}

	int level::get_width() const
	{
		return tiles.shape()[1];
	}

	int level::get_height() const
	{
		return tiles.shape()[0];
	}

	tile& level::get_tile_by_index(ivec2 idx)
	{
		return tiles[get_height() - idx.y - 1][idx.x];
	}

	tile const& level::get_tile_by_index(ivec2 idx) const
	{
		return tiles[get_height() - idx.y - 1][idx.x];
	}

	tile& level::get_tile_by_pos(vec2 pos)
	{
		const auto index = index_for_pos(pos);
		return get_tile_by_index(index);
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
		if (idx.x >= get_width() || idx.y >= get_height()) return false;
		return true;
	}

}}}
