#include "core/system/game.h"
#include "core/physics/physics_manager.h"
#include <easylogging++.h>
#include <chrono>
#include "core/rendering/render_manager.h"

using namespace std;

namespace eversim { namespace core { namespace system {

	game::game() : main_camera("main_camera", {1920,1080}, 20.f)
	{
		set_clock(make_shared<utility::realtime_clock>());
	}

	game::game(physics::physics_manager* physics, shared_ptr<utility::clock> game_clock)
		: game()
	{
		EVERSIM_ASSERT(physics);
		set_clock(game_clock);
		set_physics(physics);
	}

	void game::set_physics(physics::physics_manager* phy)
	{
		EVERSIM_ASSERT(phy);
		physics = phy;
	}

	void game::set_graphics(rendering::render_manager* ren)
	{
		EVERSIM_ASSERT(ren);
		rendering = ren;
	}

	void game::set_clock(utility::clock::ptr clk)
	{
		EVERSIM_ASSERT(clk);
		game_clock = clk;
		frame_started = game_clock->now();
	}

	gameobject* game::create_empty()
	{
		return objects.emplace(gameobject::creation_key{}, this);
	}

	void game::kill_object(gameobject* o)
	{
		if (o == nullptr) return;
		o->alive = false;
		dead_objects.insert(o);
		for(auto& child : o->children)
		{
			kill_object(child);
		}
	}

	void game::step_frame()
	{
		const auto now = game_clock->now();
		const auto time_passed = now - frame_started;
		frame_started = game_clock->now();


		update_gameobjects(time_passed);
		step_physics(time_passed);
		post_physics_update();

		cleanup_dead();
	}

	void game::render()
	{
		if(!rendering)
		{
			LOG(WARNING) << "Call to render with no render_manager attached!";
			return;
		}
		rendering->clear();
		rendering->draw(main_camera);
	}

	void game::cleanup_dead()
	{
		for(auto& o : dead_objects)
		{
			objects.erase(objects.locate(o));
		}
	}

	void game::step_physics(utility::clock::duration time_passed)
	{
		if (!physics) return;

		using namespace std::chrono;
		time_accumulator += time_passed;
		auto num_iterations = 0;
		while(time_accumulator > timestep)
		{
			const auto dt =
				float(duration_cast<milliseconds>(timestep).count())
				/ float(duration_cast<milliseconds>(1s).count());

			physics->integrate(dt);
			time_accumulator -= timestep;

			num_iterations++;

			if(num_iterations == 10)
			{
				break;
			}
		}
		LOG_IF(num_iterations > 3, INFO) << "Took " << num_iterations << " iterations";
	}

	void game::update_gameobjects(utility::clock::duration time_passed)
	{
		for(auto& obj : objects)
		{
			obj.set_frametime(time_passed);
			obj.update();
		}
	}

	void game::post_physics_update()
	{
		for(auto& o : objects)
		{
			o.post_physics_update();
		}
	}

	utility::clock::time_point game::get_time() const
	{
		return frame_started;
	}


}}}
