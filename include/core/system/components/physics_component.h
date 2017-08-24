#pragma once
#include "core/system/component.h"

namespace eversim {namespace core {namespace physics {
	class body_template;
	class body;
	class physics_manager;
}}}

namespace eversim {namespace core { namespace system {
	
	class physics_component : public component {
	public:

		explicit physics_component(
			gameobject* owner, physics::physics_manager&, physics::body_template&
		);

		physics::body& get_body() { return *body; }
		physics::body const& get_body() const { return *body; }

		static physics_component* get_from_body(physics::body const*);
		static gameobject& gameobject_from_body(physics::body const*);

	protected:
		std::unique_ptr<component> do_clone() const override
		{
			return std::make_unique<physics_component>(*this);
		}

	public:
		void update() override;
		void post_physics_update() override;
	private:
		physics::body* body;
	};

} } }
