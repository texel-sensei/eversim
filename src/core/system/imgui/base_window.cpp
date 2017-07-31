#include "core/system/imgui/base_window.h"
#include <imgui/imgui.h>

namespace eversim { namespace core { namespace system { namespace imgui {
	base_window::base_window(std::string const& n, bool vis)
		: name(n), visible(vis)
	{
	}

	void base_window::draw(display_type type)
	{
		header(type);
		draw_content();
		footer(type);
	}

	void base_window::header(display_type type)
	{
		switch(type)
		{
		case display_type::window: 
			ImGui::Begin(name.c_str(), &visible, flags);
			break;
		case display_type::child_window: 
			ImGui::BeginChild(name.c_str());
			break;
		case display_type::inplace: 
			ImGui::PushID(name.c_str());
			break;
		default: ;
		}
	}

	void base_window::footer(display_type type)
	{
		switch(type)
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
		default: ;
		}
	}


}}}}
