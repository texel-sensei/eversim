#pragma once

#include "core/rendering/switch_viewport.h"
#include "core/rendering/texture.h"
#include <glm/glm.hpp>

namespace eversim {
	namespace core {
		namespace rendering {
			class Framebuffer {
			private:
				bool valid, with_depth;
				glm::ivec2 resolution;
				GLuint tex, fbo, depth;
				Texture color_tex0;

				void swap_members(Framebuffer& other);
			public:
				Framebuffer();
				Framebuffer(const glm::ivec2& resolution,
							const bool with_depth = false);
				Framebuffer(Framebuffer&& buffer);

				~Framebuffer();

				Framebuffer& operator=(Framebuffer&& buffer);

				void bind();
				
				const glm::ivec2 viewport() const;
				const GLuint get_fbo_id() const;
				const GLuint get_tex_id() const;
				const GLuint get_depth_id() const;
			};
		}
	}
}