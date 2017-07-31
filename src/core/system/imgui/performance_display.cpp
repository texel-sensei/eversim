#include "core/system/imgui/performance_display.h"
#include "core/system/imgui/plot_var.h"
#include "core/utility/time/print.h"

#include <imgui/imgui.h>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/max_element.hpp>


namespace eversim { namespace core { namespace system { namespace imgui {
	using namespace utility;


	performance_display::performance_display(std::string const& name, bool visible)
		: base_window(name, visible)
	{
		flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
	}

	void performance_display::compact_display(bool enable)
	{
		compact = enable;
	}

	void performance_display::register_time(std::string const& name, clock::duration time)
	{
		timings[name] = time;
	}

	scoped_timer::reporter_function performance_display::get_reporter(std::string name)
	{
		return [this, name = move(name)](clock::duration time)
		{
			register_time(name, time);
		};
	}

	void performance_display::draw_content()
	{
		clock::duration sum{};
	
		for (auto& p : timings)
		{
			auto& name = p.first;
			auto& time = p.second;

			sum += time;

			draw_time(name, time);
		}
		ImGui::Separator();
		draw_time("sum", sum);

		timings.clear();
	}

	void performance_display::draw_time(std::string const& name, clock::duration time) const
	{
		using namespace std::chrono;
		if (compact)
		{
			ImGui::Text("%s: %s", name.c_str(), to_string(time).c_str());
		} else
		{
			const auto float_time = float(duration_cast<microseconds>(time).count())/1000.f;
			
			ImGui::PlotVar(name.c_str(), float_time);
		}
	}
}}}}
