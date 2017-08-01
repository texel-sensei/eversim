#pragma once
#include "core/system/imgui/base_window.h"

#include "core/utility/time/clock.h"
#include "core/utility/time/scoped_timer.h"

#include <map>

namespace eversim {namespace core {namespace system {namespace imgui {
	
	class performance_display : public base_window {
	public:
		explicit performance_display(
			std::string const& name = "timings", bool visible = true
		);

		void compact_display(bool enable);

		void register_time(std::string const& name, utility::clock::duration time);
		utility::scoped_timer::reporter_function get_reporter(std::string name);

		bool has_menu() const override { return true; }
		void draw_menu() override;

	protected:
		void begin_window() override;
		void draw_content() override;
	private:
		bool compact = true;
		std::map<std::string, utility::clock::duration> timings;

		void draw_time(
			std::string const& name, utility::clock::duration time
		) const;
	};

} } } }
