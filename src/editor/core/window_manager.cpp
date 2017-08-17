#include "editor/core/window_manager.h"
#include <boost/range/adaptor/map.hpp>

namespace eversim { namespace editor { namespace core {

	window_manager::window_manager()
	{
	}

	void window_manager::begin_frame() const
	{
		if (is_menu_visible())
			draw_menu();
	}

	void window_manager::end_frame()
	{
		for (auto& w : windows | boost::adaptors::map_values)
		{
			if (w->is_visible())
				w->draw();
		}
	}

	void window_manager::draw_menu() const
	{
		ImGui::BeginMainMenuBar();
		if (ImGui::BeginMenu("windows"))
		{
			for (auto& w : windows | boost::adaptors::map_values)
			{
				auto const& name = w->get_name();

				bool vis = w->is_visible();
				ImGui::MenuItem(name.c_str(), nullptr, &vis);
				w->set_visible(vis);
			}
			ImGui::EndMenu();
		}

		// draw window menues
		for (auto& w : windows | boost::adaptors::map_values)
		{
			if (w->is_visible() && w->has_menu())
			{
				if (ImGui::BeginMenu(w->get_name().c_str()))
				{
					w->draw_menu();
					ImGui::EndMenu();
				}
			}
		}
		ImGui::EndMainMenuBar();
	}
}}}
