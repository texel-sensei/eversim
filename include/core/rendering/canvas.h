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
				glm::ivec2 resolution;
				glm::f32mat4 M;
				GLuint tex, fbo_tex;
				GLuint fbo;
				//GLuint vaoid, vboid;
				std::vector<canvas*> children;

				void create_framebuffer();
			public:
				explicit canvas();
				/*
				Init canvas with empty texture of size ivec2
				at position defined by the modelmatrix M
				*/
				void init(const glm::ivec2& resolution,
					const glm::f32mat4& M);
				/*
				Init canvas with texture defined by path
				at position defined by the modelmatrix M
				*/
				void init(const std::string& path,
					const glm::f32mat4& M);
				void draw(const ShaderProgram& program);
				void draw_to_texture(const ShaderProgram& program, GLuint texture);
				void add_child(canvas*);

			};

		}
	}
}