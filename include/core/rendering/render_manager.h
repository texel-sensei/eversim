#pragma once

#include "core/utility/sdl.h"
#include "core/utility/helper.h"
#include "core/rendering/renderable_entity.h"
#include "core/rendering/spritemap.h"
#include "core/rendering/texture.h"
#include "core/rendering/camera.h"

#include <glm/glm.hpp>
#include <vector>
#include <functional>

namespace eversim { namespace core { namespace rendering {

	extern class render_manager* myrenderer;
	void draw_line(glm::vec2 a, glm::vec2 b, int dur = 1);
	void draw_point(glm::vec2 p);

	//using entity_ptr std::shared_ptr<RenderableEntity>;

	class render_manager {
	public:
		explicit render_manager(glm::ivec2 resolution, bool fullscreen);

		//TODO: only temporary function
		SDL_Window* get_window() noexcept {
			return window;
		}

		void draw_line(glm::vec2 a, glm::vec2 b, int dur = 1);
		void draw_point(glm::vec2 p);

		void do_draw();
		/*
		 * Create new RenderableEntity
		 * returns the shared ptr, keeps a weak ptr
		 */
		std::shared_ptr<RenderableEntity> register_entity();
		/*
		* Create new Texture from the given path
		* utilizing the texture loader 
		* returns the shared ptr, keeps a shared ptr
		* you can safely pass them
		*/
		std::shared_ptr<Texture> register_texture(const std::string& path);
		/*
		* Create new Texture from the given path
		* utilizing the texture loader and using the given filter funtion
		* returns the shared ptr, keeps a shared ptr
		* you can safely pass them
		*/
		std::shared_ptr<Texture> register_texture(const std::string& path, std::function<void()> filtering);

		std::shared_ptr<Spritemap> register_spritemap(const size_t resolution);

		std::shared_ptr<Spritemap> register_spritemap(const size_t resolution, std::function<void()> filtering);

		/*
		 * Removes every dead weak ptr
		 * Sorts entities with the shader id
		 * draw everything
		 */
		void draw(Camera& cam);


	private:
		struct line
		{
			glm::vec2 a, b;
			int dur;
		};
		glm::ivec2 resolution;
		sdl::window_ptr window;
		sdl::context_ptr context;
		// will deinitialize SDL once an object of this class is destroyed
		utility::final_action quit_sdl;

		std::vector<line> lines;
		std::vector<glm::vec2> points;
		std::vector<std::weak_ptr<RenderableEntity>> entities;
		std::vector<std::shared_ptr<Texture>> textures;
		std::vector<std::shared_ptr<Spritemap>> spritemaps;

		void setup(bool fullscreen);
	};
	
} /* rendering */} /* core */ } /* eversim */