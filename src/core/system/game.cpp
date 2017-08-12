#include "core/system/game.h"
#include <chrono>
#include "core/physics/physics_manager.h"
#include <easylogging++.h>

namespace eversim { namespace core { namespace system {

	game::game(physics::physics_manager* physics, std::shared_ptr<utility::clock> game_clock)
		: physics(physics), game_clock(game_clock)
	{
		EVERSIM_ASSERT(physics);
		last_frame_finished = game_clock->now();
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
		const auto time_passed = now - last_frame_finished;

		update_gameobjects(time_passed);
		step_physics(time_passed);
		post_physics_update();

		cleanup_dead();

		last_frame_finished = game_clock->now();
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
		physics_time_accumulator += time_passed;
		auto num_iterations = 0;
		while(physics_time_accumulator > physics_timestep)
		{
			const auto dt =
				float(duration_cast<milliseconds>(physics_timestep).count())
				/ float(duration_cast<milliseconds>(1s).count());

			physics->integrate(dt);
			physics_time_accumulator -= physics_timestep;

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

	void game::add_resources_directory(std::string const& path)
	{
	}
}}}
