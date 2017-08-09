#pragma once
#include "core/utility/time/clock.h"
#include <glm/glm.hpp>
#include <vector>

namespace eversim {namespace core{namespace system {
	
	class gameobject {
		friend class game;
	public:		
		gameobject* get_parent() const { return parent; }

		void kill();

		template<typename Component, typename... Args>
		Component* add_component(Args&&...);

		template<typename Component>
		Component* get_component();

	private:
		game* the_game;

		gameobject* parent = nullptr;
		std::vector<gameobject*> children;

		bool alive = true;

		glm::vec2 position;
		glm::vec2 scale;
		float angle;

		explicit gameobject(game*);

		void update(utility::clock::duration);
	};

} } }
