#include "core/rendering/framebuffer.h"

#include <easylogging++.h>

namespace eversim {
	namespace core {
		namespace rendering {
			Framebuffer::Framebuffer() : valid(false)
			{}

			Framebuffer::Framebuffer(const glm::ivec2& resolution,
									const bool with_depth) :
				with_depth(with_depth),resolution(resolution)
			{
				glGenFramebuffers(1, &fbo);
				glBindFramebuffer(GL_FRAMEBUFFER, fbo);

				//Texture for framebuffer
				color_tex0 = Texture(resolution);

				if (with_depth)
				{
					glGenRenderbuffers(1, &depth);
					glBindRenderbuffer(GL_RENDERBUFFER, depth);
					glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, resolution[0], resolution[1]);
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);
				}

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_tex0.get_tex_id(), 0);
				GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
				glDrawBuffers(1, DrawBuffers);

				valid = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
				if (!valid)
					LOG(ERROR) << "Framebuffer was not initialized correct at resolution " <<
						resolution[0] << "x" << resolution[1];
				
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				
			}

			void Framebuffer::swap_members(Framebuffer& buffer)
			{
				std::swap(valid, buffer.valid);
				std::swap(with_depth, buffer.with_depth);
				std::swap(resolution, buffer.resolution);
				std::swap(tex, buffer.tex);
				std::swap(fbo, buffer.fbo);
				std::swap(depth, buffer.depth);
				std::swap(color_tex0, buffer.color_tex0);
			}

			Framebuffer::Framebuffer(Framebuffer&& buffer) noexcept
			{
				swap_members(buffer);
			}

			Framebuffer::~Framebuffer()
			{
				if (valid)
				{
					glDeleteFramebuffers(1, &fbo);
					glDeleteTextures(1, &tex);
					if(with_depth) glDeleteRenderbuffers(1, &depth);
					valid = false;
				}
			}

			Framebuffer& Framebuffer::operator=(Framebuffer&& buffer) noexcept
			{
				swap_members(buffer);
				return *this;
			}

			void Framebuffer::bind() const
			{
				glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			}

			std::shared_ptr<FramebufferAutoUnbind> Framebuffer::bind_auto_unbind() const
			{
				bind();
				return std::make_shared<FramebufferAutoUnbind>();
			}

			void Framebuffer::clear(const glm::fvec4 col) const
			{
				auto tmp = bind_auto_unbind();
				glClearColor(col[0], col[1], col[2], col[3]);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}

			glm::ivec2 Framebuffer::viewport() const
			{
				return resolution;
			}
			GLuint Framebuffer::get_fbo_id() const
			{
				return fbo;
			}
			GLuint Framebuffer::get_tex_id() const
			{
				return color_tex0.get_tex_id();
			}
			GLuint Framebuffer::get_depth_id() const
			{
				return depth;
			}
		}
	}
}