#pragma once

#include "core/utility/resource_manager.h"
#include "core/utility/plattform.h"

#include <soil/SOIL.h>
#include <glm/glm.hpp>
#include <easylogging++.h>

#include <string>
#include <vector>

namespace eversim {
	namespace core {
		namespace utility {

			struct texture_packet {
				GLuint tex_id;
				glm::ivec2 resolution;

				texture_packet(const texture_packet&) = delete;

				texture_packet(const GLuint tid, const glm::ivec2 res) :
					tex_id(tid), resolution(res)
				{}

				~texture_packet() {
					glDeleteTextures(1, &tex_id);
					LOG(INFO) << "deleted texture";
				}

				texture_packet& operator=(const texture_packet&) = delete;

				bool operator==(const texture_packet& other) const {
					return tex_id == other.tex_id;
				}
			};

			class texture_loader : public resource_manager<texture_loader, std::string, texture_packet>
			{
			public:
				std::shared_ptr<value_type> load_file(std::string const& s) const
				{
					LOG(INFO) << "Load file " << s << " to GPU";

					glm::ivec2 res;
					unsigned char* image = SOIL_load_image(s.c_str(),
						&res[0], &res[1], 0, SOIL_LOAD_RGB);

					std::vector<float> image_float(res[0] * res[1] * 3);
					for (size_t i = 0; i < image_float.size(); ++i)
						image_float[i] = static_cast<float>(image[i]) / 255.f;

					GLuint tex_id = 0;
				
					glGenTextures(1, &tex_id);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, tex_id);

					glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, res[0], res[1]);
					glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, res[0], res[1], GL_RGB, GL_FLOAT, image_float.data());

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

					glBindTexture(GL_TEXTURE_2D, 0);
				
					SOIL_free_image_data(image);

					return std::make_shared<texture_packet>(tex_id,res);
				}
			};

		}
	}
}