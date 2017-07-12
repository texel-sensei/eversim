#pragma once

#include "core/utility/plattform.h"
#include "core/utility/texture_loader.h"

#include <soil/SOIL.h>
#include <glm/glm.hpp>

#include <string>
#include <functional>

namespace {
	auto default_filtering = [](){
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	};
}

namespace eversim {
	namespace core {
		namespace rendering {
			class Texture {
			private:

				bool valid = false;
				GLuint tex_id;	
				std::shared_ptr<utility::texture_packet> tex_ptr;
				glm::ivec2 resolution;

			public:

				static utility::texture_loader loader;

				Texture();
				explicit Texture(const glm::ivec2& resolution,
					std::function<void()> filtering = default_filtering);
				explicit Texture(const std::string& string,
					std::function<void()> filtering = default_filtering);
				Texture(Texture&& other);
				Texture(Texture&) = delete;

				~Texture();

				Texture& operator=(const Texture& other) = delete;
				Texture& operator=(Texture&& other);

				const GLuint get_tex_id() const;
				const glm::ivec2 get_resolution() const;

				void bind();
				void unbind();
			};
		}
	}
}