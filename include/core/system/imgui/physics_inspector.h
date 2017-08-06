#pragma once

#include "core/system/imgui/base_window.h"
#include <unordered_map>

namespace eversim{namespace core{namespace physics {
	struct body;
	class physics_manager;
} } }

namespace eversim { namespace core { namespace system { namespace imgui {

	class physics_inspector : public base_window {

	public:
		explicit physics_inspector(
			physics::physics_manager* phy,
			std::string const& name = "physics",
			bool visible = false
		)	
			: base_window(name, visible), physics(phy)
		{
		}

	protected:
		void draw_content() override;
	private:
		physics::physics_manager* physics;
		physics::body* selected_body = nullptr;
		char name_buffer[50];
		std::unordered_map<physics::body const*, std::string> body_names;

		void display_particle_info();
		void display_body_info();

		std::string body_name(physics::body const* b) const;
	};

}}}}
