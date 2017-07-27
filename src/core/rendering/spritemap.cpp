#include "core/rendering/spritemap.h"

#include <algorithm>

using namespace eversim::core::utility;

namespace eversim {
	namespace core {
		namespace rendering {
			Spritemap::Spritemap()
			{
				glGetIntegerv(GL_MAX_TEXTURE_SIZE, &resolution[0]);
				resolution[1] = resolution[0];
				const size_t max_size = 512;
				if (resolution[0] > max_size)
					resolution = { max_size,max_size };
				init();
			}

			Spritemap::Spritemap(const size_t res) : resolution({res,res})
			{
				init();
			}

			void Spritemap::init()
			{
				canvas_tex.init(resolution);
				canvas_tex.clear();
				divider = Areadivider(resolution);
			}

			glm::ivec2 Spritemap::add_texture(ShaderProgram& program, Texture& tex)
			{
				auto pos = divider.place_rectangle(tex.get_resolution());
				
				if (pos != glm::ivec2(-1, -1))
					canvas_tex.place_texture(program, tex, pos, glm::vec2(1, 1));
				
				return pos;
			}
		}
	}
}
