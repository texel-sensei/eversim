#pragma once

#include "core/rendering/shader_program.h"
#include "core/rendering/framebuffer.h"
#include "core/rendering/texture.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>

namespace eversim {
	namespace core {
		namespace rendering {

			class canvas {
			private:
				Framebuffer fbo;
			public:
				//std::vector<canvas> children;

				explicit canvas();
				/*
				Init canvas with empty texture of size ivec2
				at position defined by the modelmatrix M
				*/
				void init(
					const glm::ivec2& resolution
				);
				/*
				Draw texture on canvas
				*/
				void place_texture(
					const ShaderProgram& program,
					Texture& texture,
					const glm::vec2 translation = glm::vec2(0, 0),
					const glm::vec2 scale = glm::vec2(1, 1));
				/*
				Clear the framebuffer
				*/
				void clear();
				void clear(const glm::fvec4& col);
				/*
				Draw the canvas without binding the framebuffer
				*/
				void draw(const ShaderProgram& program, 
					const glm::ivec2& target_resolution,
					const glm::vec2 translation = glm::vec2(0,0),
					const glm::vec2 scale = glm::vec2(1, 1));

				GLuint get_texture_id() const { return fbo.get_tex_id(); }
				Texture& get_texture() { return fbo.get_texture(); }
				glm::ivec2 get_fbo_viewport() const { return fbo.viewport(); }
				void bind_framebuffer();
				glm::ivec2 get_resolution() const { return fbo.viewport(); }
			};

		}
	}
}