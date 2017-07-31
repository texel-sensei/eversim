#pragma once


#include <glm/vec2.hpp>
#include <memory>

namespace eversim{namespace core{namespace rendering {
	class render_manager;
	class RenderableEntity;
}}}

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
	
		void size(float s)
		{
			size_ = s;
		}
	};

}}}
