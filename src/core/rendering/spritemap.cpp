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
				canvas_tex.clear({ 1.f,1.f,1.f,0.f });
				divider = Areadivider(resolution);
				set_unique_id(canvas_tex.get_texture_id());
			}

			glm::ivec2 Spritemap::add_texture(ShaderProgram& program, TextureBase& tex)
			{
				auto pos = divider.place_rectangle(tex.get_resolution());
				if (pos != glm::ivec2(-1, -1))
					canvas_tex.place_texture(program, tex, pos, glm::vec2(1, 1));
				
				return pos;
			}

			void Spritemap::bind() const
			{
				glBindTexture(GL_TEXTURE_2D, canvas_tex.get_texture_id());
			}

			glm::ivec2 Spritemap::get_resolution() const
			{
				return resolution;
			}
		}
	}
}
