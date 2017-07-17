#pragma once

#include "core/rendering/texture.h"
#include "core/utility/texture_loader.h"
#include "core/rendering/canvas.h"
#include "core/rendering/shader_program.h"

#include <easylogging++.h>
#include <soil/SOIL.h>
#include <glm/glm.hpp>

#include <iostream>
#include <string>

namespace eversim {
	namespace core {
		namespace rendering {
			class Spritemap{
			private:
				glm::ivec2 resolution;
				canvas canvas_tex;
				glm::ivec2 offset;
				size_t ymax = 0;
			public:
				Spritemap();
				void add_texture(ShaderProgram& program, Texture& tex);
				GLuint get_texture_id() const { return canvas_tex.get_texture_id(); }
			};
		}
	}
}