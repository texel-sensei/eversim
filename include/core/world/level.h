#pragma once

#include "core/world/tile.h"
#include "core/world/tile_descriptor.h"
#include "core/utility/math.h"

#include <glm/glm.hpp>
#include <boost/multi_array.hpp>
#include <unordered_set>

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
		using tile_desc_ptr = std::shared_ptr<tile_descriptor>;

		explicit level(glm::ivec2 extents, utility::array_view<tile_descriptor const*> = {});

		void initialize_graphics(rendering::render_manager&);
		void calculate_collision_shapes();

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
		utility::array_view<const utility::line> get_collision_shape(unsigned char bitset) const;

		void add_tile_descriptor(tile_desc_ptr);
	private:
		using container = boost::multi_array<tile, 2>;

		container tiles;
		float tile_size = 1.f;

		// there are 16 different collision shapes
		// collision shape with index 15 is empty, so we don't need a vector to store it
		mutable std::vector<utility::line> collision_shapes[15];

		std::unordered_set<tile_desc_ptr> tile_descriptors;
	};

}}}
