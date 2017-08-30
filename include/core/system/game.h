#pragma once
#include "core/system/gameobject.h"

#include "core/rendering/camera.h"

#include "core/utility/time/realtime_clock.h"
#include "core/utility/time/clock.h"
#include "core/utility/object_pool.h"

#include <unordered_set>

namespace eversim {namespace core {
	namespace physics {
		class physics_manager;
	}
	namespace rendering {
		class render_manager;
	}
}}

namespace eversim {namespace core {namespace system {
	using namespace std::chrono_literals;

	class game {
	public:
		game();
		explicit game(
			physics::physics_manager* physics,
			utility::clock::ptr game_clock = std::make_shared<utility::realtime_clock>()
		);

		void set_physics(physics::physics_manager*);
		void set_graphics(rendering::render_manager*);

		/*
		 * Assigns a new clock to the game. This function also updates the current
		 * time of the game with the current time of the clock
		 */
		void set_clock(utility::clock::ptr);

		/*
		 * Creates a new gameobject without any components or children.
		 * The gameobject is at the origin of the world, with a scale of (1,1) and 
		 * faces east (angle == 0).
		 */
		gameobject* create_empty();

		/*
		 * Markes a gameobject for deletion. The gameobject and all it's children are
		 * deleted with the next call to `cleanup_dead()`.
		 */
		void kill_object(gameobject*);

		/*
		 * Advances the simulation of the game world by a single frame.
		 * The duration of a frame is given by the variable `timestep`.
		 */
		void step_frame();

		/*
		 * Renders the game world into the current buffer. 
		 * This function does nothing, if the game was created without a render_manager.
		 */
		void render();

		size_t get_num_gameobjects() const { return objects.size(); }

		void cleanup_dead();

		void step_physics(utility::clock::duration time_passed);
		void update_gameobjects(utility::clock::duration time_passed);
		void post_physics_update();

		/*
		 * Returns the current time. The returned value stays the same for the whole frame.
		 */
		utility::clock::time_point get_time() const;
	private:
		physics::physics_manager* physics = nullptr;
		rendering::render_manager* rendering = nullptr;
		std::shared_ptr<utility::clock> game_clock;

		rendering::Camera main_camera;

		utility::object_pool<gameobject> objects;
		std::unordered_set<gameobject*> dead_objects;

		utility::clock::time_point frame_started;
		utility::clock::duration timestep = 8ms;
		utility::clock::duration time_accumulator = 0ms;

	};

} } }
