#include "core/rendering/texture.h"
#include "core/rendering/render_manager.h"

#include "core/world/tile_descriptor.h"
#include "core/world/level_loader.h"
#include "core/world/level.h"

#include "core/physics/constraints/angle_constraint.h"
#include "core/physics/body_template.h"
#include "core/physics/constraints/distance_constraint.h"
#include "core/physics/physics_manager.h"

#include "editor/windows/log_window.h"
#include "editor/windows/performance_display.h"
#include "editor/windows/physics_inspector.h"

#include "core/input/contextloader.h"

#include "editor/core/window_manager.h"

#include <easylogging++.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <imgui_impl_sdl_gl3.h>
#include "core/input/inputhandler.h"
#undef main

INITIALIZE_EASYLOGGINGPP

using namespace std;
using namespace eversim;
using namespace eversim::core;

world::tile_descriptor dirt, grass, bricks;
std::shared_ptr<input::InputHandler> inputhandler_ptr = nullptr;

bool direction_pressed[4];

bool handle_keypress(SDL_Keysym sym, bool down)
{
	switch (sym.sym)
	{
	case SDLK_q:
	case SDLK_ESCAPE:
		return false;
	case SDLK_w:
		direction_pressed[0] = down;
		break;
	case SDLK_a:
		direction_pressed[1] = down;
		break;
	case SDLK_s:
		direction_pressed[2] = down;
		break;
	case SDLK_d:
		direction_pressed[3] = down;
		break;
	}
	return true;
}

bool handle_sdl_events()
{
	SDL_Event event;
	auto should_continue = true;
	auto& io = ImGui::GetIO();

	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSdlGL3_ProcessEvent(&event);

		inputhandler_ptr->handle_event(event);

		auto skip_key = io.WantCaptureKeyboard || io.WantTextInput;
		switch (event.type)
		{
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

	inputhandler_ptr->execute();

	return should_continue;
}

namespace {
	const char* glTypeToString(GLenum type)
	{
		switch (type)
		{
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

	const char* glSeverityToString(GLenum severity)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_LOW:
			return "LOW";
		case GL_DEBUG_SEVERITY_MEDIUM:
			return "MEDIUM";
		case GL_DEBUG_SEVERITY_HIGH:
			return "HIGH";
		}
		return "UNKNOWN";
	}

	void enable_gl_debug()
	{
		//Enable Debugging
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_FALSE);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
		glDebugMessageCallback(
			[](
				GLenum source, GLenum type, GLuint /*id*/, GLenum severity,
				GLsizei /*length*/,	const GLchar* message, const void* /*userParam*/)
			{
				LOG(WARNING) << "GL Debug Message" << endl;
				LOG(WARNING) << "GL Debug Message" << endl;
				LOG(WARNING) << string(80, '=') << endl;
				LOG(WARNING) << "src: " << source << endl;
				LOG(WARNING) << "type: " << glTypeToString(type) << endl;
				LOG(WARNING) << "severity: " << glSeverityToString(severity) << endl;
				LOG(WARNING) << message << endl;
				LOG(WARNING) << string(80, '=');

				if (
					severity == GL_DEBUG_SEVERITY_HIGH
					&& type == GL_DEBUG_TYPE_ERROR
				)
				{
					throw 42;
				}
			},
			nullptr
		);
	}
}

glm::vec2 get_input_speed(float speedmult, float dt)
{
	auto vel = glm::vec2{};
	if (direction_pressed[0])
		vel += glm::vec2{0.f, 3.f};
	if (direction_pressed[1])
		vel += glm::vec2{-1.f, 0.f};
	if (direction_pressed[2])
		vel += glm::vec2{0.f, -1.f};
	if (direction_pressed[3])
		vel += glm::vec2{1.f, 0.f};
	return vel * speedmult * dt;
}

void initialize_tiles(world::level_loader& loader)
{
	dirt.name = "dirt";
	dirt.collision = world::collision_type::solid;
	dirt.texture_name = "dirt0.png";

	grass.name = "grass";
	grass.collision = world::collision_type::solid;
	grass.texture_name = "grass.png";

	bricks.name = "bricks";
	bricks.collision = world::collision_type::solid;
	bricks.texture_name = "stone_brick1.png";

	loader.register_tile_descriptor(&dirt);
	loader.register_tile_descriptor(&grass);
	loader.register_tile_descriptor(&bricks);
}

struct loaders {
	rendering::texture_loader* tex;
	world::level_loader* lev;
	physics::body_template_loader* bdy;

	void add_directory(string const& path) const
	{
		tex->add_search_directory(path + "/sprites");
		lev->add_search_directory(path + "/levels");
		bdy->add_search_directory(path + "/physics");
	}
};

void init_rendering(rendering::render_manager& renderer)
{
	// setup dirty hack renderer global
	rendering::myrenderer = &renderer;
}

int main(int argc, char* argv[])
{
	START_EASYLOGGINGPP(argc, argv);

	// init rendering
	const auto resolution = glm::ivec2(1920, 1080);
	auto renderer = rendering::render_manager(resolution, false);
	init_rendering(renderer);

	// initialize imgui
	ImGui_ImplSdlGL3_Init(renderer.get_window());
	auto d_im = utility::finally(ImGui_ImplSdlGL3_Shutdown);
	auto& io = ImGui::GetIO();
	io.DisplaySize.x = float(resolution.x);
	io.DisplaySize.y = float(resolution.y);

	//init inputhandler

	inputhandler_ptr = std::make_shared<input::InputHandler>("../resources/inputmaps/contexts.json");
	inputhandler_ptr->push_context("game"); 



	inputhandler_ptr->get_context("game")->register_function(
		input::InputConstants::button::DLEFT,
		[](input::InputContext& context) { LOG(INFO) << "pressed DPAD LEFT"; }
	);

	inputhandler_ptr->get_context("game")->register_function(
		input::InputConstants::button::DRIGHT,
		[](input::InputContext& context) { LOG(INFO) << "pressed DPAD RIGHT"; }
	);

	inputhandler_ptr->get_context("game")->register_function(
		input::InputConstants::button::DUP,
		[](input::InputContext& context) { LOG(INFO) << "pressed DPAD UP";	}
	);

	inputhandler_ptr->get_context("game")->register_function(
		input::InputConstants::button::DDOWN,
		[](input::InputContext& context) { LOG(INFO) << "pressed DPAD DOWN"; }
	);

	inputhandler_ptr->get_context("game")->register_function(
		input::InputConstants::state::DUCK,
		[](input::InputContext& context) { LOG(INFO) << "pressed DUCK"; }
	);

	inputhandler_ptr->get_context("game")->register_function(
		input::InputConstants::state::GOLEM,
		[](input::InputContext& context) { LOG(INFO) << "pressed GOLEM"; }
	);



	// create loaders
	world::level_loader level_loader;
	initialize_tiles(level_loader);

	physics::body_template_loader body_loader;
	body_loader.register_factory("distance", make_unique<physics::distance_constraint_factory>());
	body_loader.register_factory("angle", make_unique<physics::angle_constraint_factory>());

	// add search directories	
	auto ldrs = loaders{&rendering::Texture::loader, &level_loader, &body_loader};
	ldrs.add_directory("../resources");
	ldrs.add_directory("./resources");

	// load level
	const auto level = level_loader.load("example_level");
	level->set_tile_size(0.5f);
	level->initialize_graphics(renderer);

	// initialize physics
	physics::physics_manager physics;
	physics.register_constraint_types<
		physics::distance_constraint, physics::angle_constraint
	>();

	physics.set_level(level.get());

	// Create editor windows
	editor::core::window_manager windows;
	auto* pd = windows.add_window<editor::windows::performance_display>();
	windows.add_window<editor::windows::log_window>();
	windows.add_window<editor::windows::physics_inspector>(&physics);

	// 0. time
	const auto dt = 1.f / 60.f;

	// prepare player
	//	1. physics
	const auto player_body_template = body_loader.load("cube.bdy");
	auto* player = physics.add_body(*player_body_template, {16.f, 28.f}, 0.1f);

	//	2. rendering
	auto player_entity = renderer.add_entity();
	auto kobold = renderer.add_texture("brick_gray0\\big_kobold.png");
	player_entity->set_Texture(kobold);
	player_entity->set_Position(player->position);

	inputhandler_ptr->get_context("game")->register_function(
		input::InputConstants::range::STEER_X,
		[&](input::InputContext& context, double value) {
		//LOG(INFO) << "pressed STEER_X " << value;
		player->velocity += 3.f*dt*glm::vec2(value, 0);
	}
	);

	inputhandler_ptr->get_context("game")->register_function(
		input::InputConstants::range::STEER_Y,
		[&](input::InputContext& context, double value) {
		//LOG(INFO) << "pressed STEER_Y " << -value;
		player->velocity += 3.f*dt*glm::vec2(0,-value);
	}
	);

	inputhandler_ptr->get_context("game")->register_function(
		input::InputConstants::button::JUMP,
		[&](input::InputContext& context) 
	{
		player->velocity += 500.f*dt*glm::vec2(0,1);
	}
	);

	// setup camera
	rendering::Camera cam("default_cam",
	                      resolution,
	                      20.f);
	cam.set_position(player->position);

	// do neccessary gl setup
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	enable_gl_debug();

	// game loop
	while (handle_sdl_events())
	{

		// initialize frame for editor
		ImGui_ImplSdlGL3_NewFrame(renderer.get_window());
		windows.begin_frame();

		// read input speed
		player->velocity += get_input_speed(2.f, dt);

		{
			// do physics

			utility::scoped_timer tim(pd->get_reporter("Physics loop"));
			physics.integrate(dt);
		}

		// debug draw physics TODO: move into physics inspector!
		physics.draw_constraints();
		for (auto&& p : physics.get_particles())
		{
			rendering::draw_point(p.pos);
		}

		// pan camera to player
		const auto old_cam_pos = cam.get_position();
		cam.set_position(mix(player->position, old_cam_pos, 0.9f));

		// move player sprite to physics object
		player_entity->set_Position(player->position - glm::vec2(0.5));

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.623, 0.76, 0.729, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		{
			// draw game
			utility::scoped_timer tim(pd->get_reporter("Render loop"));
			renderer.draw(cam);
		}

		{
			// draw debug informations
			utility::scoped_timer tim(pd->get_reporter("debug rendering"));
			renderer.do_draw(cam);
		}

		{
			// draw gui

			// It is not possible to time the window rendering and display it in the same frame
			// so we just display each frame the time the last frame took
			static utility::clock::duration old_time;
			utility::scoped_timer tim([](auto t) {old_time = t; });

			pd->register_time("window rendering", old_time);
			windows.end_frame();
			ImGui::Render();
		}

		SDL_GL_SwapWindow(renderer.get_window());
	}
}
