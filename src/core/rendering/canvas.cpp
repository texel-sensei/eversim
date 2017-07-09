#include "core/rendering/canvas.h"
#include "core/rendering/switch_viewport.h"

#include <soil/SOIL.h>


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
				Texture& texture, const glm::ivec2 position
			)
			{
				program.use();

				bind_framebuffer();

				switch_viewport tmp(0, 0, fbo.viewport()[0], fbo.viewport()[1]);

				GLint loc = glGetUniformLocation(program.getID(), "tex");
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture.get_tex_id());
				glUniform1i(loc, 0);

				loc = glGetUniformLocation(program.getID(), "window");
				glUniform2f(loc, fbo.viewport()[0], fbo.viewport()[1]);

				loc = glGetUniformLocation(program.getID(), "size");
				glUniform2f(loc, texture.get_resolution()[0], texture.get_resolution()[1]);

				loc = glGetUniformLocation(program.getID(), "position");
				glUniform2f(loc, position[0], position[1]);

				glDrawArrays(GL_POINTS, 0, 1);

				glBindTexture(GL_TEXTURE_2D, 0);

				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				glUseProgram(0);
			}

			void canvas::draw(const ShaderProgram& program, 
				const glm::ivec2& target_resolution)
			{

				program.use();

				GLint loc = glGetUniformLocation(program.getID(), "tex");
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, fbo.get_tex_id());
				glUniform1i(loc, 0);

				loc = glGetUniformLocation(program.getID(), "window");
				glUniform2f(loc, target_resolution[0], target_resolution[1]);

				loc = glGetUniformLocation(program.getID(), "size");
				glUniform2f(loc, fbo.viewport()[0], fbo.viewport()[1]);

				//TODO position !
				loc = glGetUniformLocation(program.getID(), "position");
				glUniform2f(loc, 0, 0);

				glDrawArrays(GL_POINTS, 0, 1);

				glBindTexture(GL_TEXTURE_2D, 0);

				glUseProgram(0);
			}

			void canvas::bind_framebuffer()
			{
				fbo.bind();
			}

			void canvas::clear()
			{
				fbo.bind();
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glClearColor(0.1, 0.1, 0.1, 1);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
		}
	}
}
