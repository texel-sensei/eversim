#include "core/rendering/render_manager.h"
#include "core/utility/helper.h"
#include "core/system/programsequencer.h"
#include "core/system/program_gui.h"

#include "core/rendering/canvas.h"
#include "core/rendering/shader_program.h"

#include "imgui/imgui_impl_sdl_gl3.h"
#include <soil/SOIL.h>

#include <easylogging++.h>
#include <imgui/imgui.h>
#include <GL/glew.h>

#undef main

INITIALIZE_EASYLOGGINGPP

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

namespace {
	const char* glTypeToString(GLenum type) {
		switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			return "ERROR";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			return "DEPRECATED_BEHAVIOR";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			return "UNDEFINED_BEHAVIOR";
		case GL_DEBUG_TYPE_PORTABILITY:
			return "PORTABILITY";
		case GL_DEBUG_TYPE_PERFORMANCE:
			return "PERFORMANCE";
		case GL_DEBUG_TYPE_OTHER:
			return "OTHER";
		}
		return "UNKNOWN";
	}

	const char* glSeverityToString(GLenum severity) {
		switch (severity) {
		case GL_DEBUG_SEVERITY_LOW:
			return "LOW";
		case GL_DEBUG_SEVERITY_MEDIUM:
			return "MEDIUM";
		case GL_DEBUG_SEVERITY_HIGH:
			return "HIGH";
		}
		return "UNKNOWN";
	}
}

int main(int argc, char* argv[]) {
	START_EASYLOGGINGPP(argc, argv);

	LOG(INFO) << "Hello World!";
	auto resolution = glm::ivec2(1920, 1080);

	auto renderer = rendering::render_manager(resolution, false);
	auto window = renderer.get_window();

	ImGui_ImplSdlGL3_Init(window);
	auto d_im = utility::finally(ImGui_ImplSdlGL3_Shutdown);
	auto& io = ImGui::GetIO();
	io.DisplaySize.x = resolution.x;
	io.DisplaySize.y = resolution.y;

	//Enable Debugging
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_FALSE);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
	glDebugMessageCallback([](
		GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
		const GLchar* message, const void* userParam)
	{
		LOG(INFO) << "GL Debug Message" << endl;
		LOG(INFO) << "GL Debug Message" << endl;
		LOG(INFO) << string(80, '=') << endl;
		LOG(INFO) << "src: " << source << endl;
		LOG(INFO) << "type: " << glTypeToString(type) << endl;
		LOG(INFO) << "severity: " << glSeverityToString(type) << endl;
		LOG(INFO) << message << endl;
		LOG(INFO) << string(80, '=');
	},
		nullptr
		);

	/*eversim::core::rendering::canvas canvas;
	canvas.init(resolution,
		glm::ivec2(0,0));

	std::string path("..\\resources\\sprites\\brick_gray0\\brick_gray0.png");
	eversim::core::rendering::canvas texture;
	texture.init(path);

	std::string conjuration("..\\resources\\sprites\\brick_gray0\\conjuration.png");
	eversim::core::rendering::canvas conjuration_texture;
	conjuration_texture.init(conjuration);*/

	eversim::core::rendering::Texture brickwall("..\\resources\\sprites\\brick_gray0\\brick_gray0.png");
	eversim::core::rendering::canvas empty_canvas;
	empty_canvas.init(glm::ivec2(1920,1080));

	rendering::ShaderProgram program("simple quad shader");
	program.create();
	program.attach
	({
		{ "..\\resources\\shader\\screen_sized_quad_vertex.glsl",GL_VERTEX_SHADER },
		{ "..\\resources\\shader\\screen_sized_quad_geometry.glsl" , GL_GEOMETRY_SHADER },
		{ "..\\resources\\shader\\screen_sized_quad_fragment.glsl",GL_FRAGMENT_SHADER }
	});
	program.link();

	int cnt = 0;
	while(handle_sdl_events())
	{
		ImGui_ImplSdlGL3_NewFrame(window);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		/*program.use();

		conjuration_texture.draw(program, resolution);
		texture.draw_to_fbo(program);

		//conjuration_texture.draw_to_canvas(program,texture);

		glUseProgram(0);*/

		empty_canvas.clear();
		empty_canvas.place_texture(program, brickwall, glm::ivec2(cnt++, 0));
		empty_canvas.place_texture(program, brickwall, glm::ivec2(128, 128));
		empty_canvas.place_texture(program, brickwall, glm::ivec2(640, 640));
		empty_canvas.draw(program,resolution);

		//render
		ImGui::ShowTestWindow();
		ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiSetCond_Once);
		ImGui::SetNextWindowSize(ImVec2(640, 640), ImGuiSetCond_Once);
		ImGui::Begin("Game");
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImVec2 size = ImGui::GetWindowSize();
		//cout << size.x << "/" << size.y << endl;
			ImGui::GetWindowDrawList()->AddImage(
				(void*)(brickwall.get_tex_id()),
				ImVec2(
					pos.x,
					pos.y  
				)
				, 
				ImVec2(pos.x + size.x, pos.y + size.y)
			);

		ImGui::End();

		ImGui::Render();

		SDL_GL_SwapWindow(window);
	}
	return 0;
}