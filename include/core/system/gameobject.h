#pragma once
#include "core/utility/time/clock.h"
#include "core/system/errors.h"
#include "core/system/component.h"
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <memory>
#include <typeindex>

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
		component* add_component(std::unique_ptr<component> comp);

		template<typename Component>
		Component* get_component();
		component* get_component(std::type_index);

		template<typename Component>
		Component const* get_component() const;
		component const* get_component(std::type_index) const;
		

		template<typename Component>
		bool has_component() const;
		bool has_component(std::type_index) const;

		int get_num_components() const { return components.size(); }

	private:
		game* the_game;

		gameobject* parent = nullptr;
		std::vector<gameobject*> children;
		std::map<std::type_index, std::unique_ptr<component>> components;

		bool alive = true;

		glm::vec2 position;
		glm::vec2 scale{1.f,1.f};
		float angle = 0.f;


		void update(utility::clock::duration);
	};

	template <typename Component, typename ... Args>
	Component* gameobject::add_component(Args&&... args)
	{
		auto const* c = get_component<Component>();
		if(c)
		{
			EVERSIM_THROW(game_error::DuplicateComponent, typeid(Component).name());
		}
		auto ptr = std::make_unique<Component>(this, std::forward<Args>(args)...);
		auto ret = ptr.get();
		components[typeid(Component)] = move(ptr);
		return ret;
	}

	template <typename Component>
	Component* gameobject::get_component()
	{
		return static_cast<Component*>(get_component(typeid(Component)));
	}

	template <typename Component>
	Component const* gameobject::get_component() const
	{
		return static_cast<Component const*>(get_component(typeid(Component)));
	}

	template <typename Component>
	bool gameobject::has_component() const
	{
		return get_component<Component>();
	}

} } }
