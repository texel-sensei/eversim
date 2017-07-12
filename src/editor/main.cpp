#include "core/rendering/render_manager.h"
#include "core/physics/physics_manager.h"
#include "core/physics/constraints/distance_constraint.h"
#include "core/utility/helper.h"

#include <imgui/imgui_impl_sdl_gl3.h>

#include <easylogging++.h>
#include <imgui/imgui.h>
#include <GL/glew.h>
#include "core/physics/constraints/angle_constraint.h"

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
			if (skip_key)
				break;
			should_continue &= handle_keypress(event.key.keysym, true);
			break;
		case SDL_KEYUP:
			if (skip_key)
				break;
			should_continue &= handle_keypress(event.key.keysym, false);
			break;
		default:
			break;
		}
	}
	return should_continue;
}

class floor_constraint : public physics::constraint {
public:

	explicit floor_constraint(float height)
		: constraint(1), height(height)
	{
		type = physics::constraint_type::inequality;
	}

	float operator()() const override
	{
		return particles[0]->projected_position.y - height;
	}

	vector<glm::vec2> grad() const override
	{
		return {glm::vec2{0.f,1.f}};
	}

private:
	float height;
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
	rendering::draw_line({-1.f,floor_height}, {1.f,floor_height}, 99999999);

	physics::physics_manager physics;

	while (handle_sdl_events())
	{
		ImGui_ImplSdlGL3_NewFrame(window);
		//prepare frame
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// do rendering stuff
		ImGui::ShowTestWindow();


		static bool autostep = false;


		static bool render_all = true;
		ImGui::Checkbox("Render all constraints", &render_all);
		if (render_all)
		{
			physics.draw_constraints();
		}

		static float dt = 1 / 60.f;
		ImGui::InputFloat("dt", &dt);

		static bool atomic_steps = false;
		ImGui::Checkbox("Atomic stepping", &atomic_steps);

		if (atomic_steps || !physics.finished_frame())
		{
			if(ImGui::Button("atomic step"))
			{
				physics.atomic_step(dt);
			}
		} else
		{
			ImGui::Checkbox("Autostep", &autostep);

			if (autostep)
			{
				physics.integrate(dt);
			} else
			{
				if (ImGui::Button("step"))
				{
					physics.integrate(dt);
				}
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
