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
				const glm::ivec2& res,
				const glm::ivec2 pos)
			{
				this->resolution = res;
				this->position = pos;

				glGenTextures(1, &tex);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, tex);

				std::vector<unsigned char> image(resolution[0] * resolution[1] * 3,40);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
					resolution[0], resolution[1], 0, GL_RGB, GL_UNSIGNED_BYTE, image.data());

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

				glBindTexture(GL_TEXTURE_2D, 0);

				fbo = Framebuffer(resolution);
			}

			void canvas::init(
				const std::string& path,
				const glm::ivec2 position)
			{
				
				if (!std::experimental::filesystem::exists(path))
					throw std::exception("File not found");

				this->position = position;

				glGenTextures(1, &tex);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, tex);

				unsigned char* image = SOIL_load_image(path.c_str(), 
					&resolution[0], &resolution[1], 0, SOIL_LOAD_RGB);

				LOG(INFO) << "Create Texture " << path << " with size " << resolution[0] << "/" <<
					resolution[1];

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, resolution[0], resolution[1], 0, GL_RGB, GL_UNSIGNED_BYTE, image);

				SOIL_free_image_data(image);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

				glBindTexture(GL_TEXTURE_2D, 0);

				fbo = Framebuffer(resolution);
			}

			void canvas::draw(const ShaderProgram& program,
				const glm::ivec2& target_resolution)
			{
				GLint loc = glGetUniformLocation(program.getID(), "tex");
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D,tex);
				glUniform1i(loc, 0);

				loc = glGetUniformLocation(program.getID(), "window");
				glUniform2f(loc, target_resolution[0], target_resolution[1]);

				loc = glGetUniformLocation(program.getID(), "size");
				glUniform2f(loc, resolution[0], resolution[1]);

				loc = glGetUniformLocation(program.getID(), "position");
				glUniform2f(loc, position[0], position[1]);

				glDrawArrays(GL_POINTS, 0, 1);

				glBindTexture(GL_TEXTURE_2D, 0);
			}

			void canvas::draw_to_canvas(const ShaderProgram& program,
				canvas& other_canvas)
			{
				other_canvas.bind_framebuffer();

				switch_viewport tmp(0, 0, other_canvas.get_fbo_viewport()[0], other_canvas.get_fbo_viewport()[1]);

				draw(program,other_canvas.get_fbo_viewport());

				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}

			void canvas::draw_to_fbo(const ShaderProgram& program)
			{
				bind_framebuffer();

				switch_viewport tmp(0, 0, fbo.viewport()[0], fbo.viewport()[1] );
				draw(program, resolution);

				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}

			void canvas::bind_framebuffer()
			{
				fbo.bind();
			}
		}
	}
}
