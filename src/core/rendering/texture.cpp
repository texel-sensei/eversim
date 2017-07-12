#include "core/rendering/texture.h"
#include "core/utility/texture_loader.h"

#include <easylogging++.h>

#include <vector>

using std::string;
using std::vector;
using glm::ivec2;

namespace eversim {
	namespace core {
		namespace rendering {

			utility::texture_loader Texture::loader;

			Texture::Texture() : valid_empty(false)
			{}

			Texture::Texture(const ivec2& resolution,
				std::function<void()> filtering) : resolution(resolution)
			{
				glGenTextures(1, &tex_id);
				glBindTexture(GL_TEXTURE_2D, tex_id);

				vector<float> image(resolution[0] * resolution[1] * 3, 0.f);

				LOG(INFO) << "Create empty texture with size " << resolution[0] << "/" <<
					resolution[1];

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
					resolution[0], resolution[1], 0, GL_RGB, GL_FLOAT, image.data());

				filtering();

				glBindTexture(GL_TEXTURE_2D, 0);

				valid_empty = true;
			}

			Texture::Texture(const string& path,
				std::function<void()> filtering) : valid_empty(false)
			{
				unsigned char* image = SOIL_load_image(path.c_str(),
					&resolution[0], &resolution[1], 0, SOIL_LOAD_RGB);
				SOIL_free_image_data(image);

				//Let the loader create a texture with immutable storage
				//this needs opengl 4.2
				tex_ptr = loader.load_file(path);
				GLuint base_tex_id = *tex_ptr;

				//Generate the texture we want to use for the view
				glGenTextures(1, &tex_id);
				//Let opengl create a texture with the data from the immutable storage
				glTextureView(tex_id, GL_TEXTURE_2D, base_tex_id, GL_RGB8, 0, 1, 0, 1);

				//Bind for setting the filter
				glBindTexture(GL_TEXTURE_2D, tex_id);
				filtering();
				glBindTexture(GL_TEXTURE_2D, 0);
			}

			Texture::Texture(Texture&& other)
			{
				std::swap(valid_empty, other.valid_empty);
				std::swap(tex_id, other.tex_id);
				std::swap(resolution, other.resolution);
				std::swap(tex_ptr, other.tex_ptr);
			}

			Texture::~Texture()
			{
				if (valid_empty) {
					glDeleteTextures(1, &tex_id);
					valid_empty = false;
				}
			}

			Texture& Texture::operator=(Texture&& other)
			{
				std::swap(valid_empty,other.valid_empty);
				std::swap(tex_id, other.tex_id);
				std::swap(resolution, other.resolution);
				std::swap(tex_ptr, other.tex_ptr);
				return *this;
			}

			const GLuint Texture::get_tex_id() const
			{
				return tex_id;
			}

			const glm::ivec2 Texture::get_resolution() const
			{
				return resolution;
			}

			void Texture::bind()
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