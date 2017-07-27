#include "core/rendering/render_manager.h"

#include "core/physics/physics_manager.h"
#include "core/physics/body_template.h"
#include "core/physics/constraints/distance_constraint.h"
#include "core/physics/constraints/angle_constraint.h"

#include "core/utility/helper.h"
#include "core/system/programsequencer.h"
#include "core/system/program_gui.h"

#include "core/rendering/canvas.h"
#include "core/rendering/shader_program.h"
#include "core/rendering/renderable_entity.h"
#include "core/rendering/multibuffer.h"
#include "core/rendering/camera.h"
#include "core/rendering/spritemap.h"


#include <imgui/imgui_impl_sdl_gl3.h>
#include <soil/SOIL.h>

#define ELPP_FEATURE_PERFORMANCE_TRACKING
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

function<void(glm::ivec2, int)> mouse_click;

bool direction_pressed[4];

bool handle_keypress(SDL_Keysym sym, bool down)
{
	switch(sym.sym)
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
	auto& io = ImGui::GetIO();
	auto should_continue = true;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSdlGL3_ProcessEvent(&event);
		auto skip_key = io.WantCaptureKeyboard || io.WantTextInput;
		switch (event.type)
		{
		case SDL_QUIT:
			return false;
		case SDL_KEYDOWN:
			if (skip_key)
				break;
			should_continue &= handle_keypress(event.key.keysym, true);
			break;
		case SDL_KEYUP:
			if (skip_key)
				break;
			should_continue &= handle_keypress(event.key.keysym, false);
			break;
		case SDL_MOUSEBUTTONDOWN:{
			if (io.WantCaptureMouse)
				break;
			auto pos = glm::ivec2{ event.button.x, event.button.y };
			mouse_click(pos,event.button.button);
			break;
		}
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
class floor_constraint : public physics::constraint {
public:

	explicit floor_constraint(physics::body_offset_ptr p, float height)
		: constraint(1), height(height)
	{
		type = physics::constraint_type::inequality;
		particles[0] = p;
	}

	float operator()() const override
	{
		assert(particles.size() == get_arity());
		return particles[0]->projected_position.y - height;
	}

	vector<glm::vec2> grad() const override
	{
		return {glm::vec2{0.f,1.f}};
	}

private:
	float height;
};
class wall_constraint : public physics::constraint {
public:

	explicit wall_constraint(physics::body_offset_ptr p, float X)
		: constraint(1), X(X)
	{
		type = physics::constraint_type::inequality;
		particles[0] = p;
	}

	float operator()() const override
	{
		assert(particles.size() == get_arity());
		if (X > 0)
			return X - particles[0]->projected_position.x;
		else
			return particles[0]->projected_position.x - X;
	}

	vector<glm::vec2> grad() const override
	{
		return { glm::vec2{ -glm::sign(particles[0]->projected_position.x),0.f } };
	}

private:
	float X;
};

int main(int argc, char* argv[])
{
	START_EASYLOGGINGPP(argc, argv);

	LOG(INFO) << "Hello World!";
	auto resolution = glm::ivec2(1920, 1080);

	auto renderer = rendering::render_manager(resolution, false);
	auto window = renderer.get_window();

	ImGui_ImplSdlGL3_Init(window);
	auto d_im = utility::finally(ImGui_ImplSdlGL3_Shutdown);
	auto& io = ImGui::GetIO();
	io.DisplaySize.x = float(resolution.x);
	io.DisplaySize.y = float(resolution.y);

	rendering::myrenderer = &renderer;
	auto floor_height = -0.35f;

	//lukitest
	floor_height = 0;

	physics::physics_manager physics;
	physics::body_template_loader loader;

	loader.register_factory("distance", make_unique<physics::distance_constraint_factory>());
	loader.register_factory("angle", make_unique<physics::angle_constraint_factory>());
	loader.add_search_directory("../resources/physics");

	physics.register_constraint_types<
		physics::distance_constraint, physics::angle_constraint
	>();

	auto boulder_templ = loader.load("cube.bdy");

	auto add_floor_constraint = [&](physics::body* b)
	{
		for(auto& p : b->get_particles())
		{
			physics.insert_constraint(floor_constraint(
				&p, floor_height
			));
			physics.insert_constraint(wall_constraint(
				&p, -10.f
			));
			physics.insert_constraint(wall_constraint(
				&p, 10.f
			));
		}
	};

	physics::body* player;
	add_floor_constraint(player = physics.add_body(*boulder_templ, {  0.1f, 0.1f }, 1.f));
	add_floor_constraint(physics.add_body(*boulder_templ, { -.5f, 0.1f }, 0.1f));
	add_floor_constraint(physics.add_body(*boulder_templ, { 0.5f, 0.1f }, 0.1f));

	mouse_click = [&](glm::ivec2 pos, int button)
	{
		auto ss_pos = glm::vec2(pos) / glm::vec2(resolution);
		ss_pos = ss_pos*2.f - 1.f;
		ss_pos.y *= -1;
		LOG(INFO) << "Pressed " << button;

		if(button == 1) {	
			add_floor_constraint(physics.add_body(*boulder_templ, ss_pos, 0.1f));
		}
	};

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
										glm::fvec2(0,resolution[1]),
										20.f);

	eversim::core::rendering::Texture::loader.add_search_directory("..\\resources\\sprites");
	eversim::core::rendering::Texture brickwall("brick_gray0\\brick_gray0.png");
	eversim::core::rendering::Texture brickwall_big("brick_gray0\\brick_gray0_big.png");
	eversim::core::rendering::Texture conjuration("brick_gray0\\conjuration.png");
	eversim::core::rendering::Texture conjuration_big("brick_gray0\\conjuration_big.png");
	eversim::core::rendering::Texture divination("brick_gray0\\divination.png");
	eversim::core::rendering::Texture kobold("brick_gray0\\big_kobold.png");
	eversim::core::rendering::Texture biggerkobold("brick_gray0\\big_kobold_just_bigger.png");
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

	rendering::ShaderProgram textured_quad_shaderprogram("textured quad shader");
	textured_quad_shaderprogram.create();
	textured_quad_shaderprogram.attach
	({
		{ "..\\resources\\shader\\textured_quad_vertex.glsl",GL_VERTEX_SHADER },
		{ "..\\resources\\shader\\textured_quad_fragment.glsl",GL_FRAGMENT_SHADER }
	});
	textured_quad_shaderprogram.link();

	textured_quad_shaderprogram.logUnfiformslogAttributes();

	glm::fmat3 M = glm::fmat3(1.f);
	
	eversim::core::rendering::Spritemap sm(1024);
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);
	
	std::vector<eversim::core::rendering::Texture*> texes({
		&brickwall,
		&conjuration,
		&conjuration_big,
		&divination,
		&brickwall_big,
		&kobold,
		&biggerkobold
	});


	sm.add_texture(program, conjuration);

	std::uniform_int_distribution<int> distribution(0, texes.size()-1);
	
	eversim::core::rendering::Multibuffer quadbuff("quadmesh");

	quadbuff.attach
	(
	{
		{ 500,500 },
		{ 300,500 },
		{ 300,300 },
		{ 500,300 }
	}
	);
	quadbuff.attach
	(
	{
		{ 1,1 },
		{ 0,1 },
		{ 0,0 },
		{ 1,0 }
	}
	);

	quadbuff.set_draw_mode(GL_QUADS, 0, 4);
	quadbuff.create_and_upload();

	auto textured_quad = renderer.register_entity();
	{
		auto& renderablentity = *textured_quad;
		renderablentity.set_Multibuffer(&quadbuff);
		renderablentity.set_ShaderProgram(textured_quad_shaderprogram);
		renderablentity.set_Texture(sm.get_texture());
	}

	auto player_entity = renderer.register_entity();

	{
		auto& renderablentity = *player_entity;
		renderablentity.set_ShaderProgram(textured_quad_shaderprogram);
		renderablentity.set_Texture(sm.get_texture());
	}

	auto floor = renderer.register_entity();
	{
		auto& renderablentity = *floor;
		renderablentity.set_ShaderProgram(vertex_only_shaderprogram);

		renderablentity.set_Multibuffer(new eversim::core::rendering::Multibuffer("line"));
		renderablentity.get_Multibuffer()->attach(
		{
			{ -10.f,floor_height },
			{ 10.f,floor_height }
		}
		);
		renderablentity.get_Multibuffer()->attach(
		{
			{ 1,0,0,1 },
			{ 0,0,1,1 }
		}
		);
		renderablentity.get_Multibuffer()->set_draw_mode(GL_LINES, 0, 2);
		renderablentity.get_Multibuffer()->create_and_upload();
	}

	/*eversim::core::rendering::Multibuffer tribuff("trianglemesh");

	tribuff.attach
	(
	{ 
	{ 400,500 },
	{ 300,300 },
	{ 500,300 }
	}
	);
	tribuff.attach
	(
	{
		{ 0.5,0.5 },
		{ 0,0 },
		{ 1,0 }
	}
	);

	tribuff.set_draw_mode(GL_TRIANGLES, 0, 3);
	tribuff.create_and_upload();*/

	int cnt = 0;
	while(handle_sdl_events())
	{

		//cam.rotate(0.1);
		//cam.translate({ -0.5,-0.5 });
		/*if(cnt%60 ==  0)
		{
			textured_quad->set_Texture(kobold);
			textured_quad->set_Multibuffer(quadbuff);
		}
		else if(cnt%30 == 0)
		{
			textured_quad->set_Texture(sm.get_texture());
			textured_quad->set_Multibuffer(tribuff);
		}*/

		ImGui_ImplSdlGL3_NewFrame(window);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		

		auto dice_roll = distribution(generator);
		sm.add_texture(program, *(texes.at(dice_roll)));

		empty_canvas.clear();
		empty_canvas.place_texture(program, brickwall, glm::vec2(cnt++, 0), glm::vec2(3, 3));
		empty_canvas.place_texture(program, brickwall, glm::vec2(128, 128), glm::vec2(1, 1));
		empty_canvas.place_texture(program, brickwall, glm::vec2(640, 640), glm::vec2(15, 15));
		empty_canvas.place_texture(program, brickwall_linear, glm::vec2(320, 320), glm::vec2(10, 10));
		empty_canvas.place_texture(program, conjuration, glm::vec2(420, 420), glm::vec2(10, 10));
		empty_canvas.place_texture(program, biggerkobold, glm::vec2(420, 420), glm::vec2(1, 1));
		empty_canvas.draw(program,resolution, glm::vec2(0, 0), glm::vec2(1, 1));

		renderer.draw(cam);

		//render
		ImGui::ShowTestWindow();

		static float dt = 1 / 60.f;
		static bool autostep = false;

		auto vel = glm::vec2{};
		if (direction_pressed[0]) vel += glm::vec2{0.f, 3.f};
		if (direction_pressed[1]) vel += glm::vec2{ -1.f, 0.f };
		if (direction_pressed[2]) vel += glm::vec2{ 0.f, -1.f };
		if (direction_pressed[3]) vel += glm::vec2{ 1.f, 0.f };
		player->velocity += vel * dt;
		rendering::draw_point(player->position);

		static bool render_all = true;
		ImGui::Checkbox("Render all constraints", &render_all);
		if (render_all)
		{
			physics.draw_constraints();
		}

		
		ImGui::InputFloat("dt", &dt);

		static bool atomic_steps = false;
		ImGui::Checkbox("Atomic stepping", &atomic_steps);

		if (atomic_steps || !physics.finished_frame())
		{
			ImGui::Text("Next step: %s", physics.get_step_name().c_str());
			if(ImGui::Button("atomic step"))
			{
				physics.atomic_step(dt);
			}
		} else
		{
			ImGui::Checkbox("Autostep", &autostep);

			if (autostep || ImGui::Button("step"))
			{
				TIMED_SCOPE(timer, "physics simulation");
				physics.integrate(dt);
			} 
		}

		auto& renderablentity = *player_entity;
		auto PM = renderablentity.get_M();
		PM[2] = glm::fvec3(player->position-glm::fvec2(0.5,0.5),1.f);
		renderablentity.set_M(PM);

		for (auto&& p : physics.get_particles())
		{
			rendering::draw_point(p.pos);
		}

		renderer.do_draw();

		ImGui::Render();

		SDL_GL_SwapWindow(window);
	}
	return 0;
}
