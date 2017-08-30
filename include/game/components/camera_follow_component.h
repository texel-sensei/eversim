#pragma once
#include "core/system/component.h"

namespace eversim {namespace core {namespace rendering {
	class Camera;
}}}

namespace eversim { namespace game { namespace components {
	
	class camera_follow_component : public core::system::component {
	public:
		/*
		 * Makes a camera follow the owning gameobject. Don't use more than once per camera!
		 * If `cam` parameter equals nullptr, then the default game camera is used.
		 */
		explicit camera_follow_component(
			core::system::gameobject* owner, core::rendering::Camera* cam = nullptr
		);

		void post_physics_update() override;

		void set_speed(float f) { speed = f; }
		float get_speed() const { return speed; }

	protected:
		std::unique_ptr<component> do_clone() const override
		{
			return std::make_unique<camera_follow_component>(*this);
		}

	private:
		core::rendering::Camera* cam;

		/*
		 * Distance the camera moves every frame.
		 * 1.0 pans the camera instantly to the object, 0.75 moves the camera 75% of the way
		 * to the target each frame and 0.0 doesn't move the camera at all.
		 */
		float speed = 1.f;
	};

} } }
