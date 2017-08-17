#include "core/system/components/physics_component.h"
#include "core/physics/physics_manager.h"
#include "core/system/gameobject.h"

namespace eversim { namespace core { namespace system {

	physics_component::physics_component(
		gameobject* owner, physics::physics_manager& mng, physics::body_template& templ
	) : component(owner)
	{
		body = mng.add_body(templ, owner->get_position(), owner->get_scale());
	}

	void physics_component::update()
	{
		body->position = get_owner().get_position();
	}

	void physics_component::post_physics_update()
	{
		get_owner().set_position(body->position);
	}
}}}
