#include "core/system/components/rendering_component.h"
#include "core/system/gameobject.h"

#include "core/rendering/render_manager.h"
#include "core/rendering/renderable_entity.h"

#include "core/utility/matrix_helper.h"

#include <glm/gtc/matrix_transform.inl>


namespace eversim { namespace core { namespace system {


	rendering_component::rendering_component(
		gameobject* owner, rendering::render_manager& mng, std::string const& texture_name
	)
		: component(owner)
	{
		display = mng.register_entity();
		display->set_Texture(*mng.register_texture(texture_name));
		offset = -.5f*owner->get_scale();
		update_transform();
	}

	void rendering_component::post_physics_update()
	{
		update_transform();
	}

	void rendering_component::update_transform()
	{
		using namespace glm;
		using namespace utility;

		const auto& obj = get_owner();
		auto M = mat3();

		M = scale(obj.get_scale()) * M;
		M = translation(-offset) * rotation(obj.get_angle()) * translation(offset) * M;
		M = translation(obj.get_position() + offset) * M;

		display->set_M(M);
	}

}}}
