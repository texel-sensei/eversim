#include "core/system/components/physics_component.h"
#include "core/physics/physics_manager.h"
#include "core/system/gameobject.h"

namespace eversim { namespace core { namespace system {

	physics_component::physics_component(
		gameobject* owner, physics::physics_manager& mng, physics::body_template& templ
	) : component(owner)
	{
		body = mng.add_body(templ, owner->get_position(), owner->get_scale());
		body->user_data = this;
	}

	physics_component* physics_component::get_from_body(physics::body const* body)
	{
		return static_cast<physics_component*>(body->user_data);
	}

	gameobject& physics_component::gameobject_from_body(physics::body const* body)
	{
		return get_from_body(body)->get_owner();
	}

	void physics_component::update()
	{
		body->position = get_owner().get_position();
	}

	void physics_component::post_physics_update()
	{
		get_owner().set_position(body->position);
		get_owner().set_angle(body->angle);
	}
}}}
