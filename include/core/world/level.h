#pragma once

#include "core/world/tile.h"

#include <glm/glm.hpp>
#include <boost/multi_array.hpp>

namespace eversim { namespace core { namespace rendering {
	class render_manager;
}}}

namespace eversim { namespace core { namespace world {

	/*
	 * Represents a 2D array of tiles. The origin (0,0) is the bottom left corner with X and Y
	 * increasing to the right/top.
	 */
	class level {
	public:
		explicit level(glm::ivec2 extents, utility::array_view<tile_descriptor const*> = {});

		void initialize_graphics(rendering::render_manager&);

		float get_tile_size() const { return tile_size; }
		void set_tile_size(float f);

		glm::vec2 get_world_size() const;
		glm::ivec2 get_num_tiles() const;
		int get_width() const;
		int get_height() const;

		tile& get_tile_by_index(glm::ivec2);
		tile& get_tile_by_pos(glm::vec2);

		tile const& get_tile_by_index(glm::ivec2) const;
		tile const& get_tile_by_pos(glm::vec2) const;

		glm::ivec2 index_for_pos(glm::vec2) const;
		glm::vec2 center_of_tile(glm::ivec2) const;

		bool contains_index(glm::ivec2) const noexcept;
	private:
		using container = boost::multi_array<tile, 2>;

		container tiles;
		float tile_size = 1.f;
	};

}}}
