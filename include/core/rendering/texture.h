#pragma once

#include "core/utility/plattform.h"
#include "core/rendering/texture_loader.h"
#include "core/rendering/texture_base_class.h"

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
			class Texture : public TextureBase {
			private:

				bool valid = false;
				GLuint tex_id = 0;	
				std::shared_ptr<texture_packet> tex_ptr;
				glm::ivec2 resolution;

			public:

				static texture_loader loader;

				Texture();
				explicit Texture(const glm::ivec2& resolution,
					std::function<void()> filtering = default_filtering);
				explicit Texture(const std::string& string,
					std::function<void()> filtering = default_filtering);
				Texture(Texture&& other) noexcept;
				Texture(Texture&) = delete;

				~Texture();

				Texture& operator=(const Texture& other) = delete;
				Texture& operator=(Texture&& other) noexcept;

				GLuint get_tex_id() const;
				glm::ivec2 get_resolution() const override;

				void bind() const override;
				static void unbind();

				bool is_valid() const { return valid; }
				explicit operator bool() const { return valid; }
			};
		}
	}
}