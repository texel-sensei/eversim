#include "core/rendering/multibuffer.h"

#include <exception>
#include <iostream>

using namespace std;

namespace eversim {
	namespace core {
		namespace rendering {
			Multibuffer::Multibuffer(const string& name) : name(name)
			{
			}

			void Multibuffer::attach(const std::vector<glm::fvec3>& data/*, size_t elems, size_t parts*/)
			{
				//num_elems_per_type.emplace_back(elems, parts);
				arrays.push_back(data);
			}

			void Multibuffer::attach(std::vector<glm::fvec3>&& data)
			{
				arrays.push_back({});
				std::swap(arrays.back(),data);
			}

			void Multibuffer::create()
			{
				if (arrays.size() == 0)
					LOG(ERROR) << "Multibuffer can't be created with no data";

				glGenVertexArrays(1, &VaoId);
				glBindVertexArray(VaoId);

				VboIds = new GLuint[arrays.size()];

				glGenBuffers(arrays.size(), VboIds);

				glBindVertexArray(0);

				LOG(INFO) << "Created Multibuffer with " << arrays.size() << " part(s)";
				for (size_t i = 0; i < arrays.size(); ++i)
					LOG(INFO) << "Part " << i << " with size = " << arrays[i].size();
			}

			void Multibuffer::set_draw_mode(GLenum type, GLint first, GLint count)
			{
				this->type = type;
				this->first = first;
				this->count = count;
			}

			void Multibuffer::bind_and_draw() const
			{
				glBindVertexArray(VaoId);
				glDrawArrays(type, first, count);
			}

			void Multibuffer::upload()
			{
				glBindVertexArray(VaoId);

				for (size_t i = 0; i < arrays.size(); i++)
				{
					glBindBuffer(GL_ARRAY_BUFFER, VboIds[i]);

					auto& data = arrays.at(i);

					glBufferData(GL_ARRAY_BUFFER,
						data.size() * sizeof(glm::fvec3),
						data.data(), GL_STATIC_DRAW);

					glVertexAttribPointer(i,3, GL_FLOAT, GL_FALSE, 0, 0);
					glEnableVertexAttribArray(i);

					glBindBuffer(GL_ARRAY_BUFFER, 0);
				}

				glBindVertexArray(0);
			}

		}
	}
}