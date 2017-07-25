#pragma once


#include <glm/vec2.hpp>
#include <memory>


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

	private:
		friend class level;
		glm::ivec2 idx;
		float size_;
		tile_descriptor const* descriptor = &blank_tile;

		void size(float s)
		{
			size_ = s;
		}
	};

}}}
