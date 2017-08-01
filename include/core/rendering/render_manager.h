#pragma once

#include "core/utility/sdl.h"
#include "core/utility/helper.h"
#include "core/rendering/renderable_entity.h"
#include "core/rendering/spritemap.h"
#include "core/rendering/texture.h"
#include "core/rendering/camera.h"
#include "core/rendering/shader_storage_buffer.h"

#include <glm/glm.hpp>
#include <vector>
#include <functional>

namespace eversim { namespace core { namespace rendering {

	extern class render_manager* myrenderer;
	void draw_line(glm::vec2 a, glm::vec2 b, int dur = 1);
	void draw_point(glm::vec2 p);

	using entity_shptr = std::shared_ptr<RenderableEntity>;
	using entity_wkptr = std::weak_ptr<RenderableEntity>;

	using texture_shptr = std::shared_ptr<Texture>;
	using texture_wkptr = std::weak_ptr<Texture>;

	using spritemap_shptr = std::shared_ptr<Spritemap>;
	using spritemap_wkptr = std::weak_ptr<Spritemap>;

	using filter = std::function<void()>;

	class render_manager {
	public:
		explicit render_manager(glm::ivec2 resolution, bool fullscreen);

		//TODO: only temporary function
		SDL_Window* get_window() noexcept {
			return window;
		}

		void draw_line(glm::vec2 a, glm::vec2 b, int dur = 1);
		void draw_point(glm::vec2 p);

		void do_draw(Camera const& cam);
		/*
		 * Create new RenderableEntity
		 * returns the shared ptr, keeps a weak ptr
		 */
		entity_shptr register_entity(const entity_type type=DYNAMIC);
		/*
		* Create new Texture from the given path
		* utilizing the texture loader 
		* returns the shared ptr, keeps a shared ptr
		* you can safely pass them
		*/
		texture_shptr register_texture(const std::string& path);
		/*
		* Create new Texture from the given path
		* utilizing the texture loader and using the given filter funtion
		* returns the shared ptr, keeps a shared ptr
		* you can safely pass them
		*/
		texture_shptr register_texture(const std::string& path, filter filtering);

		spritemap_shptr register_spritemap(const size_t resolution);

		spritemap_shptr register_spritemap(const size_t resolution, filter filtering);

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
		std::vector<entity_wkptr> dynamic_entities;
		std::vector<entity_wkptr> static_entities;
		std::vector<entity_wkptr> freshly_added_static_entities;
		std::vector<texture_shptr> textures;
		std::vector<spritemap_shptr> spritemaps;
		std::map<Multibuffer*, shader_storage_buffer> ssbs;

		void setup(bool fullscreen);

		/*
		 * removes entities
		 * returns number of removed entities
		 */
		size_t remove_expired_entities(std::vector<entity_wkptr>&);
		void sort_entities_by_shader(std::vector<entity_wkptr>&);
		void sort_entities_by_mesh(std::vector<entity_wkptr>&);

		template<typename T>
		std::vector<std::tuple<T, size_t, size_t>> gen_blocks(
			const std::vector<entity_wkptr>& es,
			std::function<T(const RenderableEntity&)> get)
		{
			//TODO sort
			//return vector of iterators begin, end
			std::vector<std::tuple<T, size_t, size_t>> blocks;
			size_t cnt = 0;
			for (auto& wkptr : es)
			{
				auto& entity = *(wkptr.lock());

				auto id = get(entity);

				if (blocks.size() == 0)
				{
					blocks.emplace_back(id, cnt, 1);
				}
				else
				{
					auto& block = blocks.back();

					if (std::get<0>(block) == id)
						std::get<2>(block)+=1;
					else
						blocks.emplace_back(id, cnt, 1);
				}
				cnt++;
			}
			return blocks;
		}
	};
	
} /* rendering */} /* core */ } /* eversim */