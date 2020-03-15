#pragma once
#include "core/utility/time/clock.h"
#include "core/utility/math/angular.h"

#include "core/system/errors.h"
#include "core/system/component.h"

#include <glm/glm.hpp>
#include <boost/range/any_range.hpp>
#include <vector>
#include <map>
#include <memory>
#include <typeindex>

namespace eversim {namespace core{namespace system {

	class game;

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

		utility::math::orientation get_angle() const { return angle; }
		void set_angle(utility::math::orientation a) { angle = a; }

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
		
		boost::any_range<component&, boost::bidirectional_traversal_tag> components();

		template<typename Component>
		bool has_component() const;
		bool has_component(std::type_index) const;

		int get_num_components() const { return components_.size(); }

		void set_frametime(utility::clock::duration duration) { frametime = duration; }
		utility::clock::duration get_frametime() const { return frametime; }

		game& get_game() const { return *the_game; }
	private:
		game* the_game;

		gameobject* parent = nullptr;
		std::vector<gameobject*> children;
		std::map<std::type_index, std::unique_ptr<component>> components_;

		bool alive = true;

		glm::vec2 position;
		glm::vec2 scale{1.f,1.f};
		utility::math::orientation angle;

		utility::clock::duration frametime;

		void update();
		void post_physics_update();
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
		components_[typeid(Component)] = move(ptr);
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
