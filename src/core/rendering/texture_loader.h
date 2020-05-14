#pragma once

#include "core/utility/resource_manager.h"
#include "core/utility/plattform.h"

#include <lodepng/lodepng.h>
#include <glm/glm.hpp>
#include <easylogging++.h>

#include <string>
#include <vector>

namespace eversim {
	namespace core {
		namespace rendering {

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

			class texture_loader final : public utility::resource_manager<texture_loader, std::string, texture_packet>
			{
			protected:
				ptr_type load_file(std::string const& s) override
				{
					LOG(INFO) << "Load file " << s << " to GPU";

					std::vector<unsigned char> image; unsigned w, h;
					auto error = lodepng::decode( image , w , h, s.c_str(), LCT_RGBA, 8);

					if (error) {
						LOG(ERROR) << "lodepng decoding error of file " << s;
						throw std::runtime_error(("lodepng decoding error of file "+s).c_str());
					}

					glm::ivec2 res = {w,h};

					std::vector<float> image_float;//(res[0] * res[1] *4);

					auto idxf = [&](const int i ,const int j) -> size_t
					{
						return i * w + j;
					};
					for(int i = h-1; i >= 0; i--)
					{
						for(int j = 0; j < w; j++)
						{
							auto idx = idxf(i,j) * 4;
							image_float.push_back(static_cast<float>(image.at(idx)) / 255.f);
							image_float.push_back(static_cast<float>(image.at(idx+1)) / 255.f);
							image_float.push_back(static_cast<float>(image.at(idx+2)) / 255.f);
							image_float.push_back(static_cast<float>(image.at(idx+3)) / 255.f);
						}
					}

					GLuint tex_id = 0;
				
					glGenTextures(1, &tex_id);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, tex_id);

					glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, res[0], res[1]);
					glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, res[0], res[1], GL_RGBA, GL_FLOAT, image_float.data());

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

					glBindTexture(GL_TEXTURE_2D, 0);
				
					return std::make_shared<texture_packet>(tex_id,res);
				}
			};

		}
	}
}
