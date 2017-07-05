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

			void canvas::init(
				const glm::ivec2& resolution,
				const glm::f32mat4& M)
			{
				this->resolution = resolution;
				this->M = M;

				glGenTextures(1, &tex);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, tex);

				std::vector<unsigned char> image(resolution[0] * resolution[1] * 3,0.3);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
					resolution[0], resolution[1], 0, GL_RGB, GL_UNSIGNED_BYTE, image.data());

				glBindTexture(GL_TEXTURE_2D, 0);
			}

			void canvas::init(
				const std::string& path,
				const glm::f32mat4& M)
			{
				
				if (!std::experimental::filesystem::exists(path))
					throw std::exception("File not found");

				this->M = M;

				glGenTextures(1, &tex);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, tex);

				unsigned char* image = SOIL_load_image(path.c_str(), 
					&resolution[0], &resolution[1], 0, SOIL_LOAD_RGB);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, resolution[0], resolution[1], 0, GL_RGB, GL_UNSIGNED_BYTE, image);

				SOIL_free_image_data(image);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glBindTexture(GL_TEXTURE_2D, 0);
			}

			void canvas::draw()
			{
				glDrawArrays(GL_POINTS, 0, 1);
			}
		}
	}
}
