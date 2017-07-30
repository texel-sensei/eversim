#pragma once

#include "core/utility/plattform.h"
#include "core/utility/array_view.h"

namespace eversim {
	namespace core	{
		namespace rendering	{
	class buffer {
	public:
		struct empty_tag {};
		static constexpr empty_tag empty = {};

		explicit buffer(empty_tag, GLenum target);
		explicit buffer(GLenum target, utility::byte_array_view<const utility::byte> data = {}, GLenum usage = GL_DYNAMIC_DRAW);

		buffer(buffer&&) noexcept;
		buffer& operator=(buffer&&) noexcept;

		buffer(const buffer&) = delete;
		buffer& operator=(const buffer&) = delete;

		void createGLBuffer(utility::byte_array_view<const utility::byte> data = {}, GLenum usage = GL_DYNAMIC_DRAW);
		void deleteGLBuffer();

		void bind() const;

		GLuint get_native_handle() const {
			return handle;
		}

		friend void swap(buffer& a, buffer& b) noexcept;
	protected:
		~buffer();
		void update(utility::byte_array_view<const utility::byte> data, unsigned int offset);
	private:
		GLuint handle = 0;
		GLuint byte_size;
		GLenum target;
	};


		} /* rendering */
	} /* core */
} /* eversim */
