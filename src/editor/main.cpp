#include "core/rendering/render_manager.h"
#include "core/physics/physics_manager.h"
#include "core/utility/helper.h"

#include "imgui/imgui_impl_sdl_gl3.h"

#include <easylogging++.h>
#include <imgui/imgui.h>
#include <GL/glew.h>

#undef main

INITIALIZE_EASYLOGGINGPP

using namespace eversim::core;
using namespace std;

bool handle_keypress(SDL_Keysym sym, bool /*down*/)
{
	if (sym.sym == SDLK_q || sym.sym == SDLK_ESCAPE)
	{
		return false;
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

class floor_constraint : public physics::constraint<1> {
public:

	explicit floor_constraint(float height)
		: height(height)
	{
		type = physics::constraint_type::inequality;
	}

	float operator()() const override
	{
		return particles[0]->projected_position.y - height;
	}

	array<glm::vec2, arity> grad() const override
	{
		return { glm::vec2{ 0.f,1.f } };
	}

private:
	float height;
};

class distance_constraint : public physics::constraint<2> {
public:

	explicit distance_constraint(float distance)
		: distance(distance)
	{
		type = physics::constraint_type::equality;
	}

	float operator()() const override
	{
		return length(particles[0]->projected_position - particles[1]->projected_position) - distance;
	}

	array<glm::vec2, arity> grad() const override
	{
		const auto n = normalize(particles[0]->projected_position - particles[1]->projected_position);
		return{ n, -n };
	}

private:
	float distance;
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
	io.DisplaySize.x = resolution.x;
	io.DisplaySize.y = resolution.y;

	rendering::myrenderer = &renderer;
	auto floor_height = -0.35f;
	rendering::draw_line({-1.f,floor_height}, {1.f,floor_height}, 99999999);

	physics::physics_manager physics;

	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 3; j++)
		{
			physics.add_particle({{i * 0.05f - 0.25f, j * 0.1f + 0.1f},{0.f,1.f},1.f});
		}
	}
	auto& anchor = physics.get_particle(0);
	anchor.inv_mass = 0;
	anchor.vel = {};

	for(int i = 0; i < physics.get_particles().size(); ++i)
	{
		auto c = floor_constraint{ floor_height };
		c.particles[0] = &physics.get_particle(i);
		physics.add_constraint<1>(c);
	}

	for(int i = 0; i < 9; ++i)
	{
		auto& p0 = physics.get_particle(i*3);
		auto& down = physics.get_particle(i*3 + 1);
		auto& down_right = physics.get_particle((i+1) * 3 + 1);
		auto& right = physics.get_particle((i + 1) * 3);
		auto dist = [](auto& a, auto& b) {return glm::length(a.pos - b.pos); };
		auto make_constraint = [&](auto& a, auto& b)
		{
			auto c = distance_constraint{ dist(a,b) };
			c.particles = { &a, &b };
			physics.add_constraint<2>(c);
		};
		make_constraint(p0, down);
		make_constraint(p0, right);
		make_constraint(p0, down_right);
		
	}

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
		ImGui::InputFloat("dt", &dt);
		ImGui::Checkbox("Autostep", &autostep);
		if(autostep){
			physics.integrate(dt);
		}else
		{
			if(ImGui::Button("step"))
			{
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
