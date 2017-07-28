#include "core/rendering/texture.h"
#include "core/utility/texture_loader.h"

#include <easylogging++.h>

#include <vector>

using std::string;
using std::vector;
using std::swap;
using glm::ivec2;

namespace eversim {
	namespace core {
		namespace rendering {

			utility::texture_loader Texture::loader;

			Texture::Texture()
			{}

			Texture::Texture(const ivec2& resolution,
				std::function<void()> filtering) : resolution(resolution)
			{
				glGenTextures(1, &tex_id);
				glBindTexture(GL_TEXTURE_2D, tex_id);

				vector<float> image(resolution[0] * resolution[1] * 4, 0.f);
				for(size_t i = 3; i < image.size(); i+=4)
				{
					image.at(i) = 1.f;
				}

				LOG(INFO) << "Create empty texture with size " << resolution[0] << "/" <<
					resolution[1];

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
					resolution[0], resolution[1], 0, GL_RGBA, GL_FLOAT, image.data());

				filtering();

				glBindTexture(GL_TEXTURE_2D, 0);

				valid = true;
			}

			Texture::Texture(const string& path,
				std::function<void()> filtering)
			{
				//Let the loader create a texture with immutable storage
				//this needs opengl 4.2
				tex_ptr = loader.load(path);
				auto& tp = *tex_ptr;
				auto base_tex_id = tp.tex_id;
				resolution = tp.resolution;

				LOG(INFO) << "Create texture with size " << resolution[0] << "/" <<
					resolution[1];

				//Generate the texture we want to use for the view
				glGenTextures(1, &tex_id);
				//Let opengl create a texture with the data from the immutable storage
				glTextureView(tex_id, GL_TEXTURE_2D, base_tex_id, GL_RGBA8, 0, 1, 0, 1);

				//Bind for setting the filter
				glBindTexture(GL_TEXTURE_2D, tex_id);
				filtering();
				glBindTexture(GL_TEXTURE_2D, 0);

				valid = true;
			}

			Texture::Texture(Texture&& other) noexcept
			{
				swap(valid, other.valid);
				swap(tex_id, other.tex_id);
				swap(resolution, other.resolution);
				swap(tex_ptr, other.tex_ptr);
			}

			Texture::~Texture()
			{
				if (valid) {
					glDeleteTextures(1, &tex_id);
					valid = false;
				}
			}

			Texture& Texture::operator=(Texture&& other) noexcept
			{
				swap(valid,other.valid);
				swap(tex_id, other.tex_id);
				swap(resolution, other.resolution);
				swap(tex_ptr, other.tex_ptr);
				return *this;
			}

			GLuint Texture::get_tex_id() const
			{
				return tex_id;
			}

			glm::ivec2 Texture::get_resolution() const
			{
				return resolution;
			}

			void Texture::bind() const 
			{
				glBindTexture(GL_TEXTURE_2D, tex_id);
			}

			void Texture::unbind()
			{
				glBindTexture(GL_TEXTURE_2D, 0);
			}
		}
	}
}