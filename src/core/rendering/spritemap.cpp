#include "core/rendering/spritemap.h"

#include <algorithm>

using namespace eversim::core::utility;

namespace eversim {
	namespace core {
		namespace rendering {

			std::weak_ptr<ShaderProgram> Spritemap::program;

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

			glm::ivec2 Spritemap::add_texture(TextureBase& tex)
			{
				if (program.expired())
				{
					LOG(ERROR) << "The ShaderProgram of the Spritemap is expired. This means the texture will not be added.";
					return glm::ivec2(-1);
				}
					
				auto pos = divider.place_rectangle(tex.get_resolution());
				if (pos != glm::ivec2(-1)) {
					canvas_tex.place_texture(*program.lock(), tex, pos, glm::vec2(1, 1));
					placed_textures.emplace_back(pos,tex.get_resolution(),tex.get_unique_id());
				}
				
				return pos;
			}

			bool  Spritemap::contains_texture(const GLuint& uid) const
			{
				auto it = find_if(begin(placed_textures), end(placed_textures), 
					[uid](const placed_texture& pt) { return pt.uid == uid; }
				);
				return it != end(placed_textures);
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
