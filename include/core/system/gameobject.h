#pragma once
#include "core/utility/time/clock.h"
#include <glm/glm.hpp>
#include <vector>

namespace eversim {namespace core{namespace system {
	
	class gameobject {
		friend class game;
		struct creation_key{};
	public:		
		explicit gameobject(creation_key, game*);

		gameobject* get_parent() const { return parent; }

		void add_child(gameobject* child);
		void set_parent(gameobject* parent);

		glm::vec2 get_position() const { return position; }
		void set_position(glm::vec2 p) { position = p; }

		glm::vec2 get_scale() const { return scale; }
		void set_scale(glm::vec2 s) { scale = s; }

		float get_angle() const { return angle; }
		void set_angle(float a) { angle = a; }

		gameobject* clone() const;

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
		glm::vec2 scale{1.f,1.f};
		float angle = 0.f;


		void update(utility::clock::duration);
	};

} } }
