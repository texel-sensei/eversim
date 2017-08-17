#pragma once

#include "editor/core/base_window.h"

#include <unordered_map>

namespace eversim {namespace core {namespace physics {
	struct body;
	class physics_manager;
}}}


namespace eversim { namespace editor { namespace windows {

	class physics_inspector : public core::base_window {
	public:
		explicit physics_inspector(
			eversim::core::physics::physics_manager* phy,
			std::string const& name = "physics",
			bool visible = false
		)
			: base_window(name, visible), physics(phy)
		{
		}

	protected:
		void draw_content() override;
	private:
		eversim::core::physics::physics_manager* physics;
		eversim::core::physics::body* selected_body = nullptr;
		char name_buffer[50];
		std::unordered_map<eversim::core::physics::body const*, std::string> body_names;

		void display_particle_info();
		void display_body_info();

		std::string body_name(eversim::core::physics::body const* b) const;
	};

}}}
