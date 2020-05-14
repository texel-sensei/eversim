#pragma once

#include "core/world/errors.h"

#include "core/utility/array_view.h"

#include <glm/vec2.hpp>
#include <memory>
#include <bitset>

namespace eversim::core{
	namespace utility::math {
		struct line;
	}
	namespace rendering {
		class render_manager;
		class RenderableEntity;
	}
}

namespace eversim { namespace core { namespace world {

	class level;

	struct tile_descriptor;
	extern tile_descriptor blank_tile;

	struct tile {
		tile() = default;

		explicit tile(glm::ivec2 idx, float size)
			: idx(idx), size_(size)
		{
		}

		bool point_inside(glm::vec2 p) const;

		/*
		 * Returns the tile coordinates of a worldspace point.
		 * Tile coordinates inside the tile span [-1,1]^2. The center is at
		 * (0,0), the top right corner is (1,1) and the lower left corner is at (-1,-1).
		 * 
		 * Values that are >1 or <-1 mean the point lies outside the tile.
		 */
		glm::vec2 to_tile_coordinates(glm::vec2 p) const;

		// Returns nullptr if neighbour is invalid
		tile const* get_neighbour(glm::ivec2 delta) const;
		bool has_collision() const noexcept;

		/* This returns the general shape of this tile
		 * Note: The returned lines are not in the correct position!
		 * They are centered around (0,0) and not the position of the tile!
		*/
		utility::array_view<const utility::math::line> get_collision_shape() const;

		/*
		 * calculates the (internal) used 4 bit index for the collision shape.
		 * The bits are set, when there is a solid tile in the corresponding direction.
		 * The directions are (from bit[3] to bit[0]):
		 *	west, south, east, north
		 */
		std::bitset<4> calculate_collision_shape() const;

		float x() const { return idx.x*size() + size() / 2.f; }
		float y() const { return idx.y*size() + size() / 2.f; }
		glm::vec2 position() const { return {x(), y()}; }

		glm::ivec2 index() const { return idx; }

		/*
		 * This is the full width/height of the tile, from the left (top) edge to the right (bottom) edge.
		 */
		float size() const { return size_; }

		tile_descriptor const* get_descriptor() const
		{
			return descriptor;
		}

		void set_descriptor(tile_descriptor const* desc)
		{
			EVERSIM_ASSERT(desc);
			descriptor = desc;
		}

		void initialize_graphics(rendering::render_manager& mng);

	private:
		friend class level;
		level const* lvl = nullptr;
		glm::ivec2 idx;
		float size_;
		tile_descriptor const* descriptor = &blank_tile;
		std::shared_ptr<rendering::RenderableEntity> display;

		// encodes what collision shape this tile should use
		// depending on its neighbours
		std::bitset<4> collision_shape;
	
		void size(float s)
		{
			size_ = s;
		}

	};

}}}
