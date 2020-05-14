#pragma once
#include "core/system/component.h"
#include <glm/glm.hpp>
#include <string>


namespace eversim {namespace core {namespace rendering {
	class render_manager;
	class RenderableEntity;
}}}

namespace eversim {namespace core { namespace system {
	
	class rendering_component : public component {
	public:
		explicit rendering_component(gameobject* owner, rendering::render_manager&, std::string const& texture_name);

		void post_physics_update() override;

		void set_position_offset(glm::vec2 offset)
		{
			this->offset = offset;
		}

	protected:
		std::unique_ptr<component> do_clone() const override
		{
			return std::make_unique<rendering_component>(*this);
		}

	private:
		std::shared_ptr<rendering::RenderableEntity> display;
		glm::vec2 offset;

		void update_transform();
	};

} } }
