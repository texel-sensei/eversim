#pragma once
#include "core/system/component.h"
#include "core/physics/body.h"

namespace eversim {namespace core {namespace physics {
	class body_template;
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
