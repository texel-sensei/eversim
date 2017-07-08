#pragma once

#include "core/rendering/switch_viewport.h"

#include <glm/glm.hpp>

namespace eversim {
	namespace core {
		namespace rendering {
			class Framebuffer {
			private:
				bool valid, with_depth;
				glm::ivec2 resolution;
				GLuint tex, fbo, depth;
			public:
				Framebuffer();
				Framebuffer(const glm::ivec2& resolution,
							const bool with_depth = false);
				Framebuffer(const Framebuffer& buffer);
				Framebuffer(Framebuffer&& buffer);

				~Framebuffer();

				Framebuffer& operator=(const Framebuffer& buffer);
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