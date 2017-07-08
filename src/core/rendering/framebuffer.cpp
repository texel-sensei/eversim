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
				//create framebuffer
				glGenFramebuffers(1, &fbo);
				glBindFramebuffer(GL_FRAMEBUFFER, fbo);

				//Texture for framebuffer
				glGenTextures(1, &tex);

				glBindTexture(GL_TEXTURE_2D, tex);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, resolution[0], resolution[1], 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glBindTexture(GL_TEXTURE_2D, 0);

				if (with_depth)
				{
					glGenRenderbuffers(1, &depth);
					glBindRenderbuffer(GL_RENDERBUFFER, depth);
					glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, resolution[0], resolution[1]);
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);
				}

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
				GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
				glDrawBuffers(1, DrawBuffers);

				valid = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
				if (!valid)
					LOG(ERROR) << "Framebuffer was not initialized correct";
				
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}

			Framebuffer::Framebuffer(const Framebuffer& buffer) :
				Framebuffer(buffer.resolution,buffer.with_depth)
			{}

			Framebuffer::Framebuffer(Framebuffer&& buffer)
			{
				*this = buffer;
			}

			Framebuffer::~Framebuffer()
			{
				if (valid)
				{
					glDeleteFramebuffers(1, &fbo);
					glDeleteTextures(1, &tex);
					if(with_depth) glDeleteRenderbuffers(1, &depth);
				}
			}

			Framebuffer& Framebuffer::operator=(const Framebuffer& buffer)
			{
				*this = Framebuffer(buffer);
				return *this;
			}

			Framebuffer& Framebuffer::operator=(Framebuffer&& buffer)
			{
				std::swap(valid, buffer.valid);
				std::swap(with_depth, buffer.with_depth);
				std::swap(resolution, buffer.resolution);
				std::swap(tex, buffer.tex);
				std::swap(fbo, buffer.fbo);
				std::swap(depth, buffer.depth);
				return *this;
			}

			void Framebuffer::bind()
			{
				glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			}
			const glm::ivec2 Framebuffer::viewport() const
			{
				return resolution;
			}
			const GLuint Framebuffer::get_fbo_id() const
			{
				return fbo;
			}
			const GLuint Framebuffer::get_tex_id() const
			{
				return tex;
			}
			const GLuint Framebuffer::get_depth_id() const
			{
				return depth;
			}
		}
	}
}