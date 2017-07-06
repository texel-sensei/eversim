#pragma once

#include "core\rendering\shader_program.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>

namespace eversim {
	namespace core {
		namespace rendering {

			class canvas {
			private:
				glm::ivec2 resolution, position;
				GLuint tex, fbo_tex;
				GLuint fbo;
				//GLuint vaoid, vboid;


				void create_framebuffer();
			public:
				std::vector<canvas> children;

				explicit canvas();
				/*
				Init canvas with empty texture of size ivec2
				at position defined by the modelmatrix M
				*/
				void init(
					const glm::ivec2& resolution,
					const glm::ivec2& position
				);
				/*
				Init canvas with texture defined by path
				at position defined by the modelmatrix M
				*/
				void init(const std::string& path,
					const glm::ivec2& position);
				void draw(const ShaderProgram& program,
					const glm::ivec2& target_resolution
					);
				void draw_to_texture(const ShaderProgram& program, GLuint texture);
				GLuint texture_id() const { return fbo_tex; };
				//GLuint texture_id() const { return tex; };
			};

		}
	}
}