#pragma once

#include "editor/core/window_manager.h"

namespace eversim::core::system{
	class game;
}

namespace eversim::editor::core {
	
	class editor {
	public:
		void save_layout();
		void load_layout();

		void render();

		template<typename Window, typename... Args>
		Window* add_window(Args&&... args)
		{
			return windows.add_window<Window>(std::forward<Args>(args)...);
		}

		template<typename Window>
		Window* get_window()
		{
			return nullptr;
		}

	private:
		window_manager windows;
		std::shared_ptr<eversim::core::system::game> main_game;
	};

}
