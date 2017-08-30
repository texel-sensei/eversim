#include "game/components/camera_follow_component.h"
#include "core/rendering/camera.h"
#include "core/system/gameobject.h"
#include "core/error.h"
#include <easylogging++.h>

using namespace eversim::core;

namespace eversim { namespace game { namespace components {

	camera_follow_component::camera_follow_component(system::gameobject* owner, rendering::Camera* c)
		: component(owner)
	{
		if(!c)
		{
			// TODO: Implement main camera follow
			EVERSIM_THROW(generic_error::NotYetImplemented,
				"Creating a camera follow component that automatically uses the default camera is not yet implemented!");
		}else
		{
			cam = c;
		}
		cam->set_position(get_owner().get_position());
	}

	void camera_follow_component::post_physics_update()
	{
		const auto old_cam_pos = cam->get_position();
		cam->set_position(mix(old_cam_pos, get_owner().get_position(), speed));
	}
}}}
