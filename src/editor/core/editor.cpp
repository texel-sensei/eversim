#include "editor/core/editor.h"
#include "core/utility/time/scoped_timer.h"
#include "editor/windows/performance_display.h"

namespace eversim::editor::core {
	void editor::render()
	{
		auto* pd = get_window<windows::performance_display>();

		windows.begin_frame();

		if(main_game)
		{
			
		}

		{
			// draw gui

			// It is not possible to time the window rendering and display it in the same frame
			// so we just display each frame the time the last frame took
			static eversim::core::utility::clock::duration old_time;
			eversim::core::utility::scoped_timer tim([](auto t) {old_time = t; });

			pd->register_time("window rendering", old_time);
			windows.end_frame();
			ImGui::Render();
		}
	}
}
