#include "core/rendering/spritemap.h"

namespace eversim {
	namespace core {
		namespace rendering {

			Spritemap::Spritemap() : offset({0,0})
			{
				glGetIntegerv(GL_MAX_TEXTURE_SIZE, &resolution[0]);
				resolution[1] = resolution[0];
				const size_t max_size = 512;
				if (resolution[0] > max_size)
					resolution = { max_size,max_size };
				LOG(INFO) << resolution[0] << "x" << resolution[1];
				canvas_tex.init(resolution);
				canvas_tex.clear();
			}

			void Spritemap::add_texture(ShaderProgram& program, Texture& tex)
			{

				//x coord
				auto o = offset;
				auto& tr = tex.get_resolution();
				if (offset[0] + tr[0] > resolution[0]) {
					offset[1] += ymax;
					offset[0] = 0;
					ymax = 0;
					o = offset;
				}
				else {
					offset[0] += tr[0];
				}
				if (ymax < tr[1]) ymax = tr[1];

				canvas_tex.place_texture(program, tex, o, glm::vec2(1, 1));
				
			}
		}
	}
}
