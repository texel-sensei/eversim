#include "core/rendering/render_manager.h"

#include "core/physics/physics_manager.h"
#include "core/physics/body_template.h"
#include "core/physics/constraints/distance_constraint.h"
#include "core/physics/constraints/angle_constraint.h"

#include "core/world/level.h"
#include "core/world/level_loader.h"
#include "core/world/tile_descriptor.h"

#include "core/utility/helper.h"

#include <imgui/imgui_impl_sdl_gl3.h>

#define ELPP_FEATURE_PERFORMANCE_TRACKING
#include <easylogging++.h>
#include <imgui/imgui.h>
#include <GL/glew.h>


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
	auto floor_height = -0.f;
	rendering::draw_line({-1.f,floor_height}, {1.f,floor_height}, 99999999);

	world::tile_descriptor dirt;
	dirt.name = "dirt";
	dirt.collision = world::collision_type::solid;

	world::level_loader ll;
	ll.add_search_directory("../resources/levels");
	ll.register_tile_descriptor(&dirt);
	auto l = ll.load("example_level");
	l->set_tile_size(0.2f);

	physics::physics_manager physics;
	physics::body_template_loader loader;

	loader.register_factory("distance", make_unique<physics::distance_constraint_factory>());
	loader.register_factory("angle", make_unique<physics::angle_constraint_factory>());
	loader.add_search_directory("../resources/physics");

	physics.register_constraint_types<
		physics::distance_constraint, physics::angle_constraint
	>();

	physics.set_level(l.get());

	auto boulder_templ = loader.load("cube.bdy");

	auto add_floor_constraint = [&](physics::body* b)
	{
		for(auto& p : b->get_particles())
		{
			physics.insert_constraint(floor_constraint(
				&p, floor_height
			));
			physics.insert_constraint(wall_constraint(
				&p, 0.f
			));
			physics.insert_constraint(wall_constraint(
				&p, 0.9f
			));
		}
	};

	physics::body* player;
	add_floor_constraint(player = physics.add_body(*boulder_templ, {  0.4f, 0.1f }, 0.1f));
	//add_floor_constraint(physics.add_body(*boulder_templ, { -.5f, 0.1f }, 0.1f));
	//add_floor_constraint(physics.add_body(*boulder_templ, { 0.5f, 0.1f }, 0.1f));

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

	while (handle_sdl_events())
	{
		ImGui_ImplSdlGL3_NewFrame(window);
		//prepare frame
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// do rendering stuff
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
