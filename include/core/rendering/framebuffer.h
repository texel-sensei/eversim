#pragma once

#include "core/rendering/switch_viewport.h"
#include "core/rendering/texture.h"
#include <glm/glm.hpp>

namespace eversim {
	namespace core {
		namespace rendering {

			class FramebufferAutoUnbind
			{
			public:
				~FramebufferAutoUnbind()
				{
					//LOG(INFO) << "unbind";
					glBindFramebuffer(GL_FRAMEBUFFER, 0);
				}
			};

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
				void unbind() const;
				std::shared_ptr<FramebufferAutoUnbind> bind_auto_unbind() const;

				void clear(const glm::fvec4 col = glm::fvec4(1.f)) const;

				glm::ivec2 viewport() const;
				GLuint get_fbo_id() const;
				GLuint get_tex_id() const;
				GLuint get_depth_id() const;
				Texture& get_texture() { return color_tex0; };
			};
		}
	}
}