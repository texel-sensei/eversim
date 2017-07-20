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

			class Area {
			public:
				glm::ivec2 min, max;
				float size_area;

				Area() = delete;
				explicit Area(const glm::ivec2& min_pos, const glm::ivec2& max_pos);

				bool operator==(const Area& other) const;

				std::vector<Area> place_rectangle(const glm::ivec2& size);
				float if_it_fits_i_sits(const glm::ivec2& size) const;
			};

			std::ostream& operator<<(std::ostream& out,const Area& a);
			
			class Spritemap{
			private:
				glm::ivec2 resolution;
				canvas canvas_tex;
				glm::ivec2 offset;
				size_t ymax = 0;

				std::vector<Area> free_areas;
			public:
				Spritemap();
				void add_texture(ShaderProgram& program, Texture& tex);
				GLuint get_texture_id() const { return canvas_tex.get_texture_id(); }
			};
		}
	}
}