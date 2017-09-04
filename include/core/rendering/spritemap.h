#pragma once

#include "core/rendering/texture_base_class.h"
#include "core/rendering/texture.h"
#include "core/rendering/texture_loader.h"
#include "core/rendering/canvas.h"
#include "core/rendering/shader_program.h"
#include "core/utility/area.h"

#include <easylogging++.h>
#include <glm/glm.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <utility>

namespace eversim {
	namespace core {
		namespace rendering {

			class Spritemap : public TextureBase {

			public:
				struct placed_texture
				{
				public:
					glm::ivec2 pos, size;
					GLuint uid;

					placed_texture(const glm::ivec2& p, const glm::ivec2& s, const GLuint& i) :
						pos(p), size(s), uid(i)
					{}
				};

			private:
				glm::ivec2 resolution;
				canvas canvas_tex;

				utility::Areadivider divider;

				std::vector<placed_texture> placed_textures;

				void init();

				//static std::weak_ptr<ShaderProgram> program;
				static ShaderProgram program;

			public:
				/*
				 * Creates a Spritemap with the maximum supported resolution
				 * bound by the hardcoded maximum of 512x512
				 */
				Spritemap();
				/*
				 * Creates a quadratic Spritemap with the resolution being
				 * size x size
				 */
				explicit Spritemap(const size_t res);
				/*
				 * Add a texture to the best fitting area
				 * returns false if the spritemap has no empty space left 
				 */
				glm::ivec2 add_texture(TextureBase& tex);
				GLuint get_texture_id() const { return canvas_tex.get_texture_id(); }
				Texture& get_texture() { return canvas_tex.get_texture(); };

				void bind() const override;
				glm::ivec2 get_resolution() const override;

				const std::vector<placed_texture>& get_placed_textures() const 
				{
					return placed_textures;
				}

				bool contains_texture(const GLuint& uid) const;

				static void init_shader();
			};
		}
	}
}