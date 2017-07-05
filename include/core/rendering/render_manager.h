#pragma once

#include "core/utility/sdl.h"
#include "core/utility/helper.h"
#include "core/rendering/canvas.h"

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

	private:
		glm::ivec2 resolution;
		sdl::window_ptr window;
		sdl::context_ptr context;
		// will deinitialize SDL once an object of this class is destroyed
		utility::final_action quit_sdl;

		void setup(bool fullscreen);
	};
	
} /* rendering */} /* core */ } /* eversim */