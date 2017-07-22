#pragma once

#include "core/utility/sdl.h"
#include "core/utility/helper.h"
#include "core/rendering/canvas.h"
#include "core/rendering/renderable_entity.h"

#include <glm/glm.hpp>

#include <vector>

namespace eversim { namespace core { namespace rendering {

	class render_manager {
	public:
		explicit render_manager(glm::ivec2 resolution, bool fullscreen);

		//TODO: only temporary function
		SDL_Window* get_window() noexcept {
			return window;
		}

		/*
		 * Create new RenderableEntity
		 * returns the shared ptr, keeps a weak ptr
		 */
		std::shared_ptr<RenderableEntity> register_entity();
		/*
		 * Removes every dead weak ptr
		 * Sorts entities with the shader id
		 * draw everything
		 */
		void draw();

	private:
		glm::ivec2 resolution;
		sdl::window_ptr window;
		sdl::context_ptr context;
		// will deinitialize SDL once an object of this class is destroyed
		utility::final_action quit_sdl;

		std::vector<std::weak_ptr<RenderableEntity>> entities;

		void setup(bool fullscreen);
	};
	
} /* rendering */} /* core */ } /* eversim */