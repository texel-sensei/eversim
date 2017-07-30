#pragma once
#include "buffer.h"

namespace eversim {
	namespace core {
		namespace rendering {
	class shader_storage_buffer : public buffer {
		public:
			shader_storage_buffer(): buffer(buffer::empty, GL_SHADER_STORAGE_BUFFER){}

			explicit shader_storage_buffer(
					utility::array_view<const unsigned char> data,
					GLenum usage = GL_DYNAMIC_COPY
			) :
				buffer(GL_SHADER_STORAGE_BUFFER, data, usage)
			{}


			void bind(int location) const{
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, location, get_native_handle());
			}

			using buffer::update;
	};
		} /* rendering */
	} /* core */
} /* eversim */

