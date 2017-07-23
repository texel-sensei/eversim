#include "core/rendering/render_manager.h"

#include "core/utility/sdl.h"
#include "core/utility/helper.h"

#include <GL/glew.h>
#include <iostream>
#include <tuple>
#include <limits>

using namespace std;

#undef max

namespace eversim { namespace core { namespace rendering {

	render_manager::render_manager(glm::ivec2 resolution, bool fullscreen) :
		resolution(resolution)
	{
		setup(fullscreen);
	}


	void render_manager::setup(bool fullscreen)
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

		if (SDL_Init(SDL_INIT_VIDEO) != 0) {
			cerr << "SDL_Init Error: " << SDL_GetError();
			throw sdl::sdl_error{ "Failed to init SDL!" };
		}
		quit_sdl = &SDL_Quit;

		auto flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		if(fullscreen)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		window = sdl::make_window(
			"eversim",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			resolution.x, resolution.y,
			flags
		);

		context = SDL_GL_CreateContext(window);

		SDL_GL_MakeCurrent(window, context);

		glewExperimental = GL_TRUE;
		auto res = glewInit();
		if (res != GLEW_OK) {
			cerr << "Failed to init glew!";
			cerr << glewGetErrorString(res);
			throw sdl::sdl_error{ "Failed to init GLEW!" };
		}

	}

	std::shared_ptr<RenderableEntity> render_manager::register_entity()
	{
		auto ptr = std::shared_ptr<RenderableEntity>(new RenderableEntity);
		entities.push_back(ptr);
		return ptr;
	}

	void render_manager::draw()
	{
		auto deref = [](std::weak_ptr<RenderableEntity>& wkptr)
		{
			auto sptr = wkptr.lock();
			return sptr;
		};

		auto end_ptr = std::remove_if(begin(entities), end(entities), 
			[](std::weak_ptr<RenderableEntity>& wptr)
		{
			return wptr.expired();
		});

		std::sort(begin(entities), end_ptr, 
			[&](std::weak_ptr<RenderableEntity>& a, std::weak_ptr<RenderableEntity>& b)
		{
			auto raptr = deref(a); auto& ra = *raptr;
			auto rbptr = deref(b); auto& rb = *rbptr;

			if (ra.program == nullptr || rb.program == nullptr)
				return false;

			return ra.program->getID() < rb.program->getID();
		});

		entities = std::vector<std::weak_ptr<RenderableEntity>>(begin(entities),end_ptr);

		//shader id, start idx, num elems
		std::vector<std::tuple<GLuint, size_t, size_t>> blocks;

		size_t cnt = 0;
		for (auto& wkptr : entities)
		{
			auto entityptr = deref(wkptr); auto& entity = *entityptr;

			if (entity.program == nullptr)
				break;

			auto id = entity.program->getID();

			if (blocks.size() == 0)
			{
				blocks.emplace_back(id,cnt,1);
			} else
			{
				auto& block = blocks.back();

				if (std::get<0>(block) == id)
					std::get<2>(block)++;
				else
					blocks.emplace_back(id,cnt,1);
			}
			cnt++;
		}
		//LOG(INFO) << "number of drawable blocks = " << blocks.size();
		for(auto& block : blocks)
		{
			auto fbeptr = deref(entities.at(std::get<1>(block)));
			auto& fbe = *fbeptr;
			ShaderProgram& program = *(fbe.program);
			program.use();

			for(auto i = std::get<1>(block); i < std::get<1>(block) + std::get<2>(block) ;++i)
			{
				auto entityptr = deref(entities.at(i)); auto& entity = *entityptr;

				if (entity.cam == nullptr) continue;

				auto location = glGetUniformLocation(program.getID(), "M");
				if (location == -1)
					LOG(INFO) << "Uniform name ""M"" does not exist";
				glUniformMatrix3fv(location, 1, GL_FALSE, &entity.get_M()[0][0]);

				entity.cam->use(program);
				entity.bind();
				entity.draw();
			}

			glUseProgram(0);

		}
	}

} /* rendering */ } /* core */ } /* eversim */