#pragma once
#include "editor/core/base_window.h"

#include "core/utility/time/clock.h"
#include "core/utility/time/scoped_timer.h"

#include "core/physics/body_template.h"

#include <map>

namespace eversim { namespace editor { namespace windows {

	class physicsbody_creator : public core::base_window {
	public:
		explicit physicsbody_creator(
			std::string const& name = "physicscreator", bool visible = true
		);

		bool has_menu() const override { return true; }
	//	void draw_menu() override;

	protected:
	//	void begin_window() override;
		void draw_content() override;
	private:

		void clear();

		char body_name[32];
		std::vector<eversim::core::physics::body_template> bodies;
	};

}}}
