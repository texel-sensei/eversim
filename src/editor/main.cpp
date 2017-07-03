#include "core/test.h"
#include "core/utility/sdl.h"
#include "core/utility/helper.h"

#include "imgui/imgui_impl_sdl_gl3.h"

#include <iostream>
#include <imgui/imgui.h>
#include <GL/glew.h>

#undef main

using namespace eversim::core;
using namespace std;

bool handle_keypress(SDL_Keysym sym, bool /*down*/)
{
	if(sym.sym == SDLK_q || sym.sym == SDLK_ESCAPE)
	{
		return false;
	}
	return true;
}

bool handle_sdl_events() {
	SDL_Event event;
	auto& io = ImGui::GetIO();
	auto should_continue = true;
	while (SDL_PollEvent(&event)) {
		ImGui_ImplSdlGL3_ProcessEvent(&event);
		auto skip_key = io.WantCaptureKeyboard || io.WantTextInput;
		switch (event.type) {
		case SDL_QUIT:
			return false;
		case SDL_KEYDOWN:
			if (skip_key) break;
			should_continue &= handle_keypress(event.key.keysym, true);
			break;
		case SDL_KEYUP:
			if (skip_key) break;
			should_continue &= handle_keypress(event.key.keysym, false);
			break;
		default:
			break;
		}
	}
	return should_continue;
}

int main() {
	
	auto resolution = glm::ivec2(1920, 1080);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		cerr << "SDL_Init Error: " << SDL_GetError();
		return false;
	}
	auto d_sdl = utility::finally(SDL_Quit);

	auto window = sdl::make_window(
		"Hello World!",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		resolution.x, resolution.y,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
	);

	auto sdl_context = SDL_GL_CreateContext(window);
	if (!sdl_context) {
		cerr << "SDL context error " << SDL_GetError();
		return false;
	}
	auto d_ctx = utility::finally([sdl_context] {SDL_GL_DeleteContext(sdl_context); });

	SDL_GL_MakeCurrent(window, sdl_context);
	ImGui_ImplSdlGL3_Init(window);
	auto d_im = utility::finally(ImGui_ImplSdlGL3_Shutdown);
	auto& io = ImGui::GetIO();
	io.DisplaySize.x = resolution.x;
	io.DisplaySize.y = resolution.y;

	glewExperimental = GL_TRUE;
	auto res = glewInit();
	if (res != GLEW_OK) {
		cerr << "Failed to init glew!";
		cerr << glewGetErrorString(res);
		return false;
	}

	while(handle_sdl_events())
	{
		ImGui_ImplSdlGL3_NewFrame(window);
		//prepare frame
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// do rendering stuff
		ImGui::ShowTestWindow();

		ImGui::Render();
		SDL_GL_SwapWindow(window);
	}

	std::cout << "Editor: " << foo() << std::endl;
	return 0;
}