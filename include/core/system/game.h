#pragma once
#include "core/system/gameobject.h"

#include "core/utility/time/clock.h"
#include "core/utility/object_pool.h"
#include <unordered_set>
#include "core/utility/time/realtime_clock.h"


namespace eversim {namespace core {namespace physics {
	class physics_manager;
}}}

namespace eversim {namespace core {namespace system {
	using namespace std::chrono_literals;

	class game {
	public:
		game() = default;
		explicit game(
			physics::physics_manager* physics,
			std::shared_ptr<utility::clock> game_clock = std::make_shared<utility::realtime_clock>()
		);

		gameobject* create_empty();
		void kill_object(gameobject*);

		void step_frame();
		void add_resources_directory(std::string const& path);

		size_t get_num_gameobjects() const { return objects.size(); }

		void cleanup_dead();

		void step_physics(utility::clock::duration time_passed);
		void update_gameobjects(utility::clock::duration time_passed);
		void post_physics_update();
	private:
		physics::physics_manager* physics;
		std::shared_ptr<utility::clock> game_clock;

		utility::object_pool<gameobject> objects;
		std::unordered_set<gameobject*> dead_objects;

		utility::clock::time_point last_frame_finished;
		utility::clock::duration physics_timestep = 8ms;
		utility::clock::duration physics_time_accumulator = 0ms;

	};

} } }
