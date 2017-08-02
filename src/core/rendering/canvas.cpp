#include "core/rendering/canvas.h"
#include "core/rendering/switch_viewport.h"


#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <filesystem>

namespace eversim {
	namespace core {
		namespace rendering {
			canvas::canvas(){}

			void canvas::init(
				const glm::ivec2& res)
			{
				fbo = Framebuffer(res);
			}

			void canvas::place_texture(const ShaderProgram& program,
				TextureBase& texture, 
				const glm::vec2 translation,
				const glm::vec2 scale
			)
			{
				auto auto_fb_unbind = bind_framebuffer_auto_unbind();

				switch_viewport tmp(0, 0, fbo.viewport()[0], fbo.viewport()[1]);

				program.use();

				auto loc = glGetUniformLocation(program.getID(), "tex");
				glActiveTexture(GL_TEXTURE0);
				texture.bind();
				glUniform1i(loc, 0);

				loc = glGetUniformLocation(program.getID(), "target_resolution");
				glUniform2f(loc, fbo.viewport()[0], fbo.viewport()[1]);

				loc = glGetUniformLocation(program.getID(), "texture_size");
				glUniform2f(loc, texture.get_resolution()[0], texture.get_resolution()[1]);

				auto T = glm::translate(glm::vec3(translation,0.f));
				auto S = glm::scale(glm::vec3(scale, 1));
				auto M = T*S;

				loc = glGetUniformLocation(program.getID(), "M");
				glUniformMatrix4fv(loc,1,GL_FALSE,&M[0][0]);

				glDrawArrays(GL_POINTS, 0, 1);

				glBindTexture(GL_TEXTURE_2D, 0);

				glUseProgram(0);
			}

			void canvas::draw(const ShaderProgram& program, 
				const glm::ivec2& target_resolution,
				const glm::vec2 translation,
				const glm::vec2 scale) const
			{
				program.use();

				auto loc = glGetUniformLocation(program.getID(), "tex");
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, fbo.get_tex_id());
				glUniform1i(loc, 0);

				loc = glGetUniformLocation(program.getID(), "target_resolution");
				glUniform2f(loc, target_resolution[0], target_resolution[1]);

				loc = glGetUniformLocation(program.getID(), "texture_size");
				glUniform2f(loc, fbo.viewport()[0], fbo.viewport()[1]);

				auto T = glm::translate(glm::vec3(translation, 0.f));
				auto S = glm::scale(glm::vec3(scale, 1));
				auto M = T*S;

				loc = glGetUniformLocation(program.getID(), "M");
				glUniformMatrix4fv(loc, 1, GL_FALSE, &M[0][0]);

				glDrawArrays(GL_POINTS, 0, 1);

				glBindTexture(GL_TEXTURE_2D, 0);

				glUseProgram(0);
			}

			void canvas::bind_framebuffer() const
			{
				fbo.bind();
			}

			std::shared_ptr<FramebufferAutoUnbind> canvas::bind_framebuffer_auto_unbind() const
			{
				return fbo.bind_auto_unbind();
			}

			void canvas::clear(const glm::fvec4& col) const
			{
				fbo.clear(col);
			}
		}
	}
}
