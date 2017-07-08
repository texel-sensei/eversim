#pragma once

#include "core/rendering/shader_program.h"
#include "core/rendering/framebuffer.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>

namespace eversim {
	namespace core {
		namespace rendering {

			class canvas {
			private:
				glm::ivec2 resolution, position;
				GLuint tex;
				Framebuffer fbo;
			public:
				std::vector<canvas> children;

				explicit canvas();
				/*
				Init canvas with empty texture of size ivec2
				at position defined by the modelmatrix M
				*/
				void init(
					const glm::ivec2& resolution,
					const glm::ivec2 position = glm::ivec2(0, 0)
				);
				/*
				Init canvas with texture defined by path
				at position defined by the modelmatrix M
				*/
				void init(const std::string& path,
					const glm::ivec2 position = glm::ivec2(0,0));
				void draw(const ShaderProgram& program,
					const glm::ivec2& target_resolution
					);
				void draw_to_canvas(const ShaderProgram& program,
									canvas& other_canvas);
				void draw_to_fbo(const ShaderProgram& program);
				//void draw_to_texture(const ShaderProgram& program, GLuint texture);
				GLuint get_texture_id() const { return tex; };
				GLuint get_fbo_texture_id() const { return fbo.get_tex_id(); }
				glm::ivec2 get_fbo_viewport() const { return fbo.viewport(); }
				void bind_framebuffer();

				glm::ivec2 get_resolution() const { return resolution; }
			};

		}
	}
}