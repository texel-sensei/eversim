#pragma once
#include "core/system/gameobject.h"

#include "core/utility/time/clock.h"
#include "core/utility/object_pool.h"
#include <unordered_set>


namespace eversim {namespace core {namespace system {
	
	class game {
	public:

		gameobject* create_empty();
		void kill_object(gameobject*);

		void step_frame();
		void add_resources_directory(std::string const& path);

		size_t get_num_gameobjects() const { return objects.size(); }

		void cleanup_dead();

	private:
		std::shared_ptr<utility::clock> game_clock;
		utility::object_pool<gameobject> objects;

		std::unordered_set<gameobject*> dead_objects;
	};

} } }
