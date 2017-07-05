#include <core/rendering/canvas.h>

#include <soil/SOIL.h>


//TESTING
#include <windows.h>

#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <filesystem>

namespace eversim {
	namespace core {
		namespace rendering {
			canvas::canvas(){}

			void canvas::create_framebuffer()
			{
				//create framebuffer
				glGenFramebuffers(1, &fbo);
				glBindFramebuffer(GL_FRAMEBUFFER, fbo);
				glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex, 0);

				//Texture for framebuffer
				glGenTextures(1, &fbo_tex);
				glBindTexture(GL_TEXTURE_2D, fbo_tex);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, resolution[0], resolution[1], 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

				glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fbo_tex, 0);
				GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
				glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}

			void canvas::init(
				const glm::ivec2& resolution,
				const glm::ivec2& position)
			{
				this->resolution = resolution;
				this->position = position;

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

				create_framebuffer();
			}

			void canvas::init(
				const std::string& path,
				const glm::ivec2& position)
			{
				
				if (!std::experimental::filesystem::exists(path))
					throw std::exception("File not found");

				this->position = position;

				glGenTextures(1, &tex);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, tex);

				unsigned char* image = SOIL_load_image(path.c_str(), 
					&resolution[0], &resolution[1], 0, SOIL_LOAD_RGB);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, resolution[0], resolution[1], 0, GL_RGB, GL_UNSIGNED_BYTE, image);

				SOIL_free_image_data(image);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

				glBindTexture(GL_TEXTURE_2D, 0);

				create_framebuffer();
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
		}
	}
}
