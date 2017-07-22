#include "core/rendering/render_manager.h"
#include "core/utility/helper.h"
#include "core/system/programsequencer.h"
#include "core/system/program_gui.h"

#include "core/rendering/canvas.h"
#include "core/rendering/shader_program.h"
#include "core/rendering/renderable_entity.h"
#include "core/rendering/multibuffer.h"
#include "core/rendering/camera.h"
#include "core/rendering/spritemap.h"


#include "imgui/imgui_impl_sdl_gl3.h"
#include <soil/SOIL.h>

#include <easylogging++.h>
#include <imgui/imgui.h>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include <random>
#include <chrono>

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

	eversim::core::rendering::Camera cam("default_cam",
										glm::fvec2(0,resolution[0]),
										glm::fvec2(0,resolution[1]));

	eversim::core::rendering::Multibuffer data("testbuffer");
	data.attach(
	{ 
		{ 200,200}, 
		{ 0,200 },
		{ 0,0 },
		{ 200,0 },

		{ 700,700 },
		{ 500,700 },
		{ 500,500 },
		{ 700,500 }
	}
	);

	data.attach(
	{
		{ 1,1,1},
		{ 0,0,0 },
		{ 1,1,1 },
		{ 0,0,0 },

		{ 1,1,1 },
		{ 0,0,0 },
		{ 1,1,1 },
		{ 0,0,0 }
	}
	);
	data.set_draw_mode(GL_QUADS, 0, 8);
	data.create_and_upload();

	eversim::core::rendering::Texture::loader.add_search_directory("..\\resources\\sprites");
	eversim::core::rendering::Texture brickwall("brick_gray0\\brick_gray0.png");
	eversim::core::rendering::Texture brickwall_big("brick_gray0\\brick_gray0_big.png");
	eversim::core::rendering::Texture conjuration("brick_gray0\\conjuration.png");
	eversim::core::rendering::Texture conjuration_big("brick_gray0\\conjuration_big.png");
	eversim::core::rendering::Texture divination("brick_gray0\\divination.png");
	eversim::core::rendering::Texture brickwall_linear("brick_gray0\\brick_gray0.png",
		[]() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	});
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

	rendering::ShaderProgram vertex_only_shaderprogram("simple shader");
	vertex_only_shaderprogram.create();
	vertex_only_shaderprogram.attach
	({
		{ "..\\resources\\shader\\vertex_only_vertex.glsl",GL_VERTEX_SHADER },
		{ "..\\resources\\shader\\vertex_only_fragment.glsl",GL_FRAGMENT_SHADER }
	});
	vertex_only_shaderprogram.link();

	vertex_only_shaderprogram.logUnfiformslogAttributes();

	glm::fmat3 M = glm::fmat3(1.f);
	
	eversim::core::rendering::Spritemap sm(1024);
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);
	
	std::vector<eversim::core::rendering::Texture*> texes({
		&brickwall,
		&conjuration,
		&conjuration_big,
		&divination,
		&brickwall_big
	});

//	sm.add_texture(program, brickwall);
	//sm.add_texture(program, conjuration_big);

//	sm.add_texture(program, brickwall);
	//sm.add_texture(program, brickwall_big);
	//sm.add_texture(program, brickwall_big);
	//sm.add_texture(program, brickwall_big);
	/*for(size_t i = 0; i < 150; ++i)
	{
		sm.add_texture(program, conjuration);
		sm.add_texture(program, brickwall);
	}*/

	LOG(INFO) << "17. texture";
	sm.add_texture(program, conjuration);

	std::uniform_int_distribution<int> distribution(0, texes.size()-1);
	/*for (size_t i = 0; i < 10; ++i) {
		int dice_roll = distribution(generator);
		sm.add_texture(program, *(texes.at(dice_roll)));
	}*/
		LOG(INFO) << "sm texture id = " << sm.get_texture_id();

	auto entity_ptr = renderer.register_entity();

	int cnt = 0;
	while(handle_sdl_events())
	{

		cam.rotate(0.1);

		ImGui_ImplSdlGL3_NewFrame(window);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		renderer.draw();

		int dice_roll = distribution(generator);
		sm.add_texture(program, *(texes.at(dice_roll)));

		empty_canvas.clear();
		empty_canvas.place_texture(program, brickwall, glm::vec2(cnt++, 0), glm::vec2(3, 3));
		empty_canvas.place_texture(program, brickwall, glm::vec2(128, 128), glm::vec2(1, 1));
		empty_canvas.place_texture(program, brickwall, glm::vec2(640, 640), glm::vec2(15, 15));
		empty_canvas.place_texture(program, brickwall_linear, glm::vec2(320, 320), glm::vec2(10, 10));
		empty_canvas.place_texture(program, conjuration, glm::vec2(420, 420), glm::vec2(10, 10));
		empty_canvas.draw(program,resolution, glm::vec2(0, 0), glm::vec2(1, 1));

		vertex_only_shaderprogram.use();

		//M = glm::translate(M, glm::fvec2(-10, 0, 0));

		GLint location = glGetUniformLocation(vertex_only_shaderprogram.getID(), "M");
		if (location == -1)
			LOG(INFO) << "Uniform name ""M"" does not exist";
		glUniformMatrix3fv(location, 1, GL_FALSE, &M[0][0]);

		cam.use(vertex_only_shaderprogram);
		data.bind_and_draw();
		glUseProgram(0);

		//render
		ImGui::ShowTestWindow();
		/*ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiSetCond_Once);
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
*/
		//spritemap
		ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiSetCond_Once);
		ImGui::SetNextWindowSize(ImVec2(640, 640), ImGuiSetCond_Once);
		ImGui::Begin("Spritemap");
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImVec2 size = ImGui::GetWindowSize();
		//cout << size.x << "/" << size.y << endl;
		ImGui::GetWindowDrawList()->AddImage(
			(void*)(sm.get_texture_id()),
			ImVec2(
				pos.x,
				pos.y
			)
			,
			ImVec2(pos.x + size.x - 32, pos.y + size.y - 32)
		);


		ImGui::End();

		ImGui::Render();

		SDL_GL_SwapWindow(window);
	}
	return 0;
}