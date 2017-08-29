#include "editor/windows/log_window.h"
#include "editor/windows/performance_display.h"
#include "editor/windows/physics_inspector.h"

#include "editor/core/window_manager.h"

#include "core/rendering/texture.h"
#include "core/rendering/render_manager.h"

#include "core/world/level_loader.h"
#include "core/world/tile_loader.h"
#include "core/world/level.h"

#include "core/physics/constraints/angle_constraint.h"
#include "core/physics/body_template.h"
#include "core/physics/constraints/distance_constraint.h"
#include "core/physics/physics_manager.h"

#include "core/system/components/physics_component.h"
#include "core/system/components/rendering_component.h"
#include "core/system/game.h"
#include "core/input/contextloader.h"
#include "core/input/gamepadhandler.h"
#include "core/utility/filesystem_wrapper.h"

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

std::shared_ptr<input::InputHandler> inputhandler_ptr = nullptr;
input::GamepadHandler ghandler;

// Keep only this much logfiles around. If there are more, then the oldest ones are deleted
constexpr int NUM_LOGFILES = 10;

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
		ghandler.handle_event(event);

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

struct loaders {
	rendering::texture_loader* tex;
	world::level_loader* lev;
	world::tile_loader* til;
	physics::body_template_loader* bdy;

	void add_directory(string const& path) const
	{
		tex->add_search_directory(path + "/sprites");
		lev->add_search_directory(path + "/levels");
		til->add_search_directory(path + "/tiles");
		bdy->add_search_directory(path + "/physics");
	}
};

void init_rendering(rendering::render_manager& renderer)
{
	// setup dirty hack renderer global
	rendering::myrenderer = &renderer;
}

void init_logging(int argc, char* argv[])
{
	using namespace utility;

	START_EASYLOGGINGPP(argc, argv);
	el::Configurations default_config;
	default_config.setToDefault();

	el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
	default_config.setGlobally(el::ConfigurationType::Filename, "logs/%datetime{%Y-%M-%d %H-%m-%s}.log");

	if (fs::exists("log.conf")) {
		default_config.parseFromFile("log.conf");
	}
	el::Loggers::reconfigureAllLoggers(default_config);

	// collect all files in logs/ directory
	auto dir_begin = fs::directory_iterator("logs");
	auto dir_end = fs::directory_iterator();
	vector<fs::path> logfiles(dir_begin, dir_end);

	// remove all files that do not end in '.log'
	const auto is_no_logfile = [](fs::path const& p) { return p.extension() != ".log"; };
	logfiles.erase(remove_if(logfiles.begin(), logfiles.end(), is_no_logfile), logfiles.end());
	sort(logfiles.begin(), logfiles.end());

	if (logfiles.size() > NUM_LOGFILES)
	{
		const auto end = logfiles.begin() + NUM_LOGFILES-1;
		for (auto it = logfiles.begin(); it != end; ++it)
		{
			LOG(INFO) << "Deleting logfile " << it->string();
			fs::remove(*it);
		}
	}
}

int main(int argc, char* argv[])
{
	init_logging(argc, argv);

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
		[](input::InputContext& context) { 
		LOG(INFO) << "pressed GOLEM"; 
	}
	);


	// create loaders
	auto tile_loader = make_shared<world::tile_loader>();
	world::level_loader level_loader{tile_loader};

	physics::body_template_loader body_loader;
	body_loader.register_factory("distance", make_unique<physics::distance_constraint_factory>());
	body_loader.register_factory("angle", make_unique<physics::angle_constraint_factory>());

	// add search directories	
	auto ldrs = loaders{&rendering::Texture::loader, &level_loader, tile_loader.get(), &body_loader};
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
	physics.set_gravity({0,-10});

	// Create game
	system::game the_game(&physics);

	// Create editor windows
	editor::core::window_manager windows;
	auto* pd = windows.add_window<editor::windows::performance_display>();
	windows.add_window<editor::windows::log_window>();
	windows.add_window<editor::windows::physics_inspector>(&physics);

	// 0. time
	const auto dt = 1.f / 60.f;

	// prepare player
	const auto player_body_template = body_loader.load("cube.bdy");

	auto player = the_game.create_empty();
	player->set_position({ 16.f, 28.f });
	player->set_scale({ .5f,.5f });

	player->add_component<system::physics_component>(physics, *player_body_template);
	player->add_component<system::rendering_component>(renderer, "brick_gray0/big_kobold.png");

	inputhandler_ptr->get_context("game")->register_function(
		input::InputConstants::range::STEER_X,
		[&](input::InputContext& context, double value) {
		if (std::abs(value) - 0.2 < 0.) return;
		double a = (value < 0.) ? -1. : 1.;
		auto pc = player->get_component<system::physics_component>();
		auto& velocity = pc->get_body().velocity;
		velocity += 3.f*dt*glm::vec2(a*std::exp(std::abs(value)), 0);
	}
	);

	inputhandler_ptr->get_context("game")->register_function(
		input::InputConstants::button::JUMP,
		[&](input::InputContext& context) 
	{
		auto pc = player->get_component<system::physics_component>();
		auto& velocity = pc->get_body().velocity;
		velocity += 5.f*glm::vec2(0, 1);
		inputhandler_ptr->push_context("midjump");
	}
	);

	inputhandler_ptr->get_context("midjump")->register_function(
		input::InputConstants::button::DOUBLEJUMP,
		[&](input::InputContext& context)
	{
		auto pc = player->get_component<system::physics_component>();
		auto& velocity = pc->get_body().velocity;
		velocity += 3.5f*glm::vec2(0, 1);
		inputhandler_ptr->push_context("midjumpjumped");
	}
	);

	utility::Delegate delg;
	delg.connect([](physics::body* ptr, 
		physics::events::static_col_list const& list) 
	{
		for (const auto& sc : list)
		{
			if (std::abs(sc.normal.y - 1.) < 0.001)
			{
				//LOG(INFO) << "ground";
				inputhandler_ptr->pop({std::string("midjumpjumped") , "midjump"});
				break;
			}
		}
	}, physics.level_collision_events());

	// setup camera
	rendering::Camera cam("default_cam",
	                      resolution,
	                      20.f);
	cam.set_position(player->get_position());

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
		player->get_component<system::physics_component>()->get_body().velocity += get_input_speed(2.f, dt);

		// pan camera to player
		const auto old_cam_pos = cam.get_position();
		cam.set_position(mix(player->get_position(), old_cam_pos, 0.9f));

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.623, 0.76, 0.729, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// step game
		{	
			utility::scoped_timer tim(pd->get_reporter("Game update"));
			the_game.step_frame();
		}

		// debug draw physics TODO: move into physics inspector!
		
		for(auto&& c : physics.get_constraints())
		{
			if(c.get_arity() <= 1)
			{
				continue;
			}
			auto const& p1 = c.particles[0]->projected_position;
			auto const& p2 = c.particles[1]->projected_position;
			rendering::draw_line(p1, p2);
		}

		for (auto&& p : physics.get_particles())
		{
			rendering::draw_point(p.pos);
		}

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
