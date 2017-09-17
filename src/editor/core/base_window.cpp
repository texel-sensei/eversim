#include "editor/core/base_window.h"
#include <imgui/imgui.h>
#include "core/error.h"

namespace eversim { namespace editor { namespace core {
	base_window::base_window(std::string const& n, bool vis)
		: name(n), visible(vis)
	{
	}

	void base_window::draw(display_type type)
	{
		const auto vis = header(type);
		if (vis)
		{
			draw_content();
		}
		footer(type);
	}

	bool base_window::header(display_type type)
	{
		switch (type)
		{
		case display_type::window:
			this->begin_window();
			return ImGui::Begin(name.c_str(), nullptr, ImVec2(0, 0), alpha, flags);
		case display_type::child_window:
			return ImGui::BeginChild(name.c_str());
		case display_type::inplace:
			ImGui::PushID(name.c_str());
			return true;
		default: 
			EVERSIM_THROW(eversim::core::generic_error::InvalidEnum);
		}
	}

	void base_window::footer(display_type type)
	{
		switch (type)
		{
		case display_type::window:
			ImGui::End();
			break;
		case display_type::child_window:
			ImGui::EndChild();
			break;
		case display_type::inplace:
			ImGui::PopID();
			break;
		default: 
			EVERSIM_THROW(eversim::core::generic_error::InvalidEnum);
		}
	}


}}}
