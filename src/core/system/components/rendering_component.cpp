#include "core/system/components/rendering_component.h"
#include "core/system/gameobject.h"

#include "core/rendering/render_manager.h"
#include "core/rendering/renderable_entity.h"



namespace eversim { namespace core { namespace system {


	rendering_component::rendering_component(
		gameobject* owner, rendering::render_manager& mng, std::string const& texture_name
	)
		: component(owner)
	{
		display = mng.register_entity();
		display->set_Texture(*mng.register_texture(texture_name));
		update_transform();
	}

	void rendering_component::post_physics_update()
	{
		update_transform();
	}

	void rendering_component::update_transform()
	{
		display->set_Position(get_owner().get_position() + offset);
		display->set_Scale(get_owner().get_scale());
		// todo: rotation?!?
	}

}}}
