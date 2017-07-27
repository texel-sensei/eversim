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
				explicit Framebuffer(const glm::ivec2& resolution,
							const bool with_depth = false);
				Framebuffer(Framebuffer&& buffer) noexcept;

				~Framebuffer();

				Framebuffer& operator=(Framebuffer&& buffer) noexcept;

				void bind() const;
				
				const glm::ivec2 viewport() const;
				const GLuint get_fbo_id() const;
				const GLuint get_tex_id() const;
				const GLuint get_depth_id() const;
				Texture& get_texture() { return color_tex0; };
			};
		}
	}
}