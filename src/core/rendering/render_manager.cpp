#include "core/rendering/render_manager.h"

#include "core/utility/sdl.h"
#include "core/utility/helper.h"

#include <GL/glew.h>
#include <algorithm>
#include <iostream>

using namespace std;

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
	}

	void render_manager::draw_line(glm::vec2 a, glm::vec2 b, int dur)
	{
		lines.push_back({ a, b, dur });
	}
	void render_manager::draw_point(glm::vec2 p)
	{
		points.emplace_back(p);
	}

	void render_manager::do_draw()
	{
		glPointSize(10);
		glBegin(GL_POINTS);
		for(auto const& p : points)
		{
			glVertex2f(p.x, p.y);
		}
		glEnd();

		glBegin(GL_LINES);
		for(auto& l : lines)
		{
			glVertex2f(l.a.x, l.a.y);
			glVertex2f(l.b.x, l.b.y);
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
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
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

} /* rendering */ } /* core */ } /* eversim */