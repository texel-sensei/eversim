#include "editor/windows/game_window.h"

#include "editor/imgui/texture_display.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>


namespace eversim { namespace editor { namespace windows {
	game_window::game_window(std::string const& name, bool visible)
		: base_window(name, visible), fb({1920,1080})
	{
	}

	void game_window::draw_menu()
	{
	}

	void game_window::bind() const
	{
		fb.bind();
	}

	void game_window::unbind() const
	{
		fb.unbind();
	}

	void game_window::begin_window()
	{
	}

	void game_window::draw_content()
	{
		using namespace ImGui;
		
		auto* window = GetCurrentWindow();
		if (window->SkipItems)
			return;

		const auto texid = reinterpret_cast<ImTextureID>(fb.get_tex_id());

		const auto top_left = GetWindowContentRegionMin();
		const auto bot_right = GetWindowContentRegionMax();
		ImGui::Image(fb.get_texture(), bot_right - top_left);
	}
}}}
