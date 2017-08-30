#include "editor/imgui/texture_display.h"
#include "core/rendering/texture.h"
#include "core/utility/math.h"

using namespace std;
using namespace eversim::core::rendering;

namespace ImGui {



	void Image(Texture const& tex, ImVec2 size, bool keep_aspect)
	{
		if (keep_aspect) {
			const auto res = tex.get_resolution();

			size = eversim::core::utility::calculateAspectRatioFit(res, size);
		}
		Image(reinterpret_cast<ImTextureID>(tex.get_tex_id()), size, { 0,1 }, { 1,0 });
	}
}
