#pragma once
#include <imgui/imgui.h>

namespace eversim {namespace core {namespace rendering {
	class Texture;
}}}

namespace ImGui {
	void Image(eversim::core::rendering::Texture const&, ImVec2 size, bool keep_aspect = true);
}
