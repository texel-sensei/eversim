#include "core/rendering/buffer.h"

#include <cassert>
#include <utility>

namespace eversim {
	namespace core
	{
		namespace rendering
		{
	buffer::buffer(empty_tag, GLenum target) : target(target) {
	}

	buffer::buffer(
			GLenum target,
			eversim::core::utility::const_byte_array_view data /* = {} */,
			GLenum usage /* = GL_DYNAMIC_DRAW */
	) : buffer(buffer::empty, target) {
		createGLBuffer(data, usage);
	}
	buffer::~buffer() {
		deleteGLBuffer();
	}


	buffer::buffer(buffer&& o) noexcept : buffer(empty, o.target) {
		swap(*this, o);
	}

	buffer& buffer::operator=(buffer&& o) noexcept {
		this->deleteGLBuffer();
		swap(*this, o);
		return *this;
	}

	void swap(buffer& a, buffer& b) noexcept{
		using std::swap;
		swap(a.handle, b.handle);
		swap(a.byte_size, b.byte_size);
		swap(a.target, b.target);
	}

	void buffer::createGLBuffer(
			eversim::core::utility::const_byte_array_view data /* = {} */,
			GLenum usage /* = GL_DYNAMIC_DRAW */
	){
		assert(handle == 0);
		glGenBuffers(1, &handle);
		bind();
		glBufferData(target, data.byte_size(), data.data(), usage);
		this->byte_size = data.byte_size();
	}
	void buffer::deleteGLBuffer(){
		if(handle) {
			glDeleteBuffers(1,&handle);
			handle = 0;
		}
	}

	void buffer::update(eversim::core::utility::const_byte_array_view data, unsigned int offset){
		assert(handle != 0);
		bind();
		glBufferSubData(target, offset, data.byte_size(), data.data());
	}

	void buffer::bind() const {
		glBindBuffer(target, handle);
	}
		} /* rendering */
	} /* core */
} /* eversim */
