#pragma once

#include "core/utility/array_view.h"

#include <glm/vec2.hpp>
#include <memory>
#include <bitset>

namespace eversim{namespace core{
	namespace utility {
		struct line;
	}
	namespace rendering {
		class render_manager;
		class RenderableEntity;
	}
}}

namespace eversim { namespace core { namespace world {

	struct tile_descriptor;
	extern tile_descriptor blank_tile;

	struct tile {
		tile() = default;

		explicit tile(glm::ivec2 idx, float size)
			: idx(idx), size_(size)
		{
		}

		bool point_inside(glm::vec2 p) const;

		// Returns nullptr if neighbour is invalid
		tile const* get_neighbour(glm::ivec2 delta) const;
		bool has_collision() const noexcept;

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

		float size() const { return size_; }

		tile_descriptor const* get_descriptor() const
		{
			return descriptor;
		}

		void set_descriptor(tile_descriptor const* desc)
		{
			assert(desc);
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
