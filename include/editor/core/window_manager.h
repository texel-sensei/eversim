#pragma once

#include "editor/core/base_window.h"

#include <easylogging++.h>

#include <type_traits>
#include <memory>
#include <map>

namespace eversim { namespace editor { namespace core {

	class window_manager {
	public:
		window_manager();

		template <
			typename Window, typename... Args,
			typename = std::enable_if_t<std::is_base_of_v<base_window, Window>>
		>
		Window* add_window(Args&&... args);

		void set_menu_visible(bool b) noexcept { menu_visible = b; }
		bool is_menu_visible() const noexcept { return menu_visible; }

		void begin_frame() const;
		void end_frame();

	private:
		using window_ptr = std::unique_ptr<base_window>;
		std::map<std::string, window_ptr> windows;
		bool menu_visible = true;

		void draw_menu() const;
	};

	template <typename Window, typename ... Args, typename>
	Window* window_manager::add_window(Args&&... args)
	{
		auto w = std::make_unique<Window>(std::forward<Args>(args)...);
		auto* ret = w.get();

		auto const& name = w->get_name();
		auto it = windows.find(name);
		if (it != windows.end())
		{
			LOG(ERROR) << "Added duplicate window to window manager! \"" << name << "\"";
			throw std::runtime_error{"Duplicate window added!"};
		}
		windows.insert(it, std::make_pair(name, move(w)));

		return ret;
	}

}}}
