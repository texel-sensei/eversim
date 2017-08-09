#pragma once
#include "core/system/gameobject.h"

#include "core/utility/time/clock.h"
#include "core/utility/object_pool.h"

namespace eversim {namespace core {namespace system {
	
	class game {
	public:

		void step_frame();
		void add_resources_directory(std::string const& path);

	private:
		std::shared_ptr<utility::clock> game_clock;
		utility::object_pool<gameobject> objects;
	};

} } }
