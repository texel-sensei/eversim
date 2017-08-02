#pragma once

#include <imgui/imgui.h>
#include <string>

namespace ImGui {
	
	void BeginBarGraph();
	void BarGraphInputValue(float val, std::string const& tooltip = "");
	void EndBarGraph(ImVec2 size_arg = {});

}