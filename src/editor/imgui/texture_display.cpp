#include "editor/imgui/texture_display.h"
#include "core/rendering/texture.h"
using namespace eversim::core::rendering;

namespace ImGui {

	void Image(Texture const& tex, ImVec2 size, bool keep_aspect)
	{
		if (keep_aspect) {
			const auto res = tex.get_resolution();
			const auto aspect_ratio = float(res.x) / float(res.y);

			const auto new_aspect = float(size.x) / float(size.y);

			if(abs(new_aspect - aspect_ratio) > 0.001)
			{
				if(size.x > size.y)
				{
					size.y *= new_aspect / aspect_ratio;
				} else
				{
					size.x *=  aspect_ratio / new_aspect;
				}
			}
		}
		Image(reinterpret_cast<ImTextureID>(tex.get_tex_id()), size, { 0,1 }, { 1,0 });
	}
}
