#include "core/rendering/render_manager.h"

#include "core/utility/sdl.h"
#include "core/utility/helper.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <iostream>
#include <tuple>
#include <limits>



using namespace std;

#undef max

namespace eversim { namespace core { namespace rendering {

	class render_manager* myrenderer;
	void draw_line(glm::vec2 a, glm::vec2 b, int dur)
	{
		myrenderer->draw_line(a, b, dur);
	}
	void draw_point(glm::vec2 p)
	{
		myrenderer->draw_point(p);
	}

	render_manager::render_manager(glm::ivec2 resolution, bool fullscreen) :
		resolution(resolution)
	{
		setup(fullscreen);
		default_quadmesh_ptr = std::make_unique<Multibuffer>("default quad mesh");
		auto& default_quadmesh = *default_quadmesh_ptr;
		default_quadmesh.attach(
		{
			{ 1,1 },
			{ 0,1 },
			{ 0,0 },
			{ 1,0 },
		}
		);
		default_quadmesh.attach(
		{
			{ 1,1 },
			{ 0,1 },
			{ 0,0 },
			{ 1,0 }
		}
		);
		default_quadmesh.set_draw_mode(GL_QUADS, 0, 4);
		default_quadmesh.create_and_upload();

		default_shader.create();
		default_shader.attach
		({
			{ "..\\resources\\shader\\textured_quad_vertex.glsl",GL_VERTEX_SHADER },
			{ "..\\resources\\shader\\textured_quad_fragment.glsl",GL_FRAGMENT_SHADER }
		});
		default_shader.link();

		default_shader.logUnfiformslogAttributes();
	}

	void render_manager::draw_line(glm::vec2 a, glm::vec2 b, int dur)
	{
		lines.push_back({ a, b, dur });
	}
	void render_manager::draw_point(glm::vec2 p)
	{
		points.emplace_back(p);
	}

	void render_manager::do_draw(Camera const& cam)
	{
		const auto V = cam.get_view_matrix();
		const auto P = cam.get_projection_matrix();

		const auto VP = P * V;

		glPointSize(5);
		glBegin(GL_POINTS);
		for(auto const& p : points)
		{
			auto proj = VP * glm::vec3(p, 1);
			proj /= proj.z;

			glColor3f(1, 1, 1);
			glVertex2f(proj.x, proj.y);
		}
		glEnd();

		
		glBegin(GL_LINES);
		for(auto& l : lines)
		{
			auto a = VP * glm::vec3(l.a, 1);
			a /= a.z;
			auto b = VP * glm::vec3(l.b, 1);
			b /= b.z;
			glColor3f(1, 0, 0);
			glVertex2f(a.x, a.y);
			glVertex2f(b.x, b.y);
			l.dur--;
		}
		glEnd();
		auto zero_dur = [](auto const& l) { return l.dur <= 0; };
		lines.erase(
			std::remove_if(lines.begin(), lines.end(), zero_dur),
			lines.end()
		);
		points.clear();
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

	std::shared_ptr<Texture>  render_manager::register_texture(const std::string& path)
	{
		auto ptr = std::make_shared<Texture>(path);
		textures.push_back(ptr);
		return ptr;
	}

	std::shared_ptr<Texture>  render_manager::register_texture(const std::string& path,
		std::function<void()> filtering)
	{
		auto ptr = std::make_shared<Texture>(path,filtering);
		textures.push_back(ptr);
		return ptr;
	}

	std::shared_ptr<Spritemap>  render_manager::register_spritemap(const size_t resolution)
	{
		auto ptr = std::make_shared<Spritemap>(resolution);
		spritemaps.push_back(ptr);
		return ptr;
	}

	std::shared_ptr<Spritemap>  render_manager::register_spritemap(const size_t resolution,
		std::function<void()> filtering)
	{
		LOG(ERROR) << "TODO spritemap with filterfunction";
		auto ptr = std::make_shared<Spritemap>(resolution);
		spritemaps.push_back(ptr);
		return ptr;
	}


	void render_manager::draw(Camera& cam)
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
				continue;

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
			//LOG(INFO) << "bind program " << program.name;
			cam.use(program);
			for(auto i = std::get<1>(block); i < std::get<1>(block) + std::get<2>(block) ;++i)
			{
				auto entityptr = deref(entities.at(i)); RenderableEntity& entity = *entityptr;

				auto location = glGetUniformLocation(program.getID(), "M");
				if (location == -1)
					LOG(INFO) << "Uniform name ""M"" does not exist";
				glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(entity.get_M()));

				entity.bind();
				entity.draw();
			}

			glUseProgram(0);

		}
	}

} /* rendering */ } /* core */ } /* eversim */