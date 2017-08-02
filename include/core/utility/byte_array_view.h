#pragma once

#include <cstdint>

namespace eversim { namespace core { namespace utility {

	enum byte : uint8_t {};

	struct const_byte_array_view {
		const_byte_array_view() : data_(nullptr), n(0) {}
		const_byte_array_view(byte const* ptr, size_t n) : data_(ptr), n(n) {}

		template<typename Cont>
		const_byte_array_view(Cont const& c) : 
			const_byte_array_view(reinterpret_cast<byte const*>(c.data()), c.size() * sizeof(*c.data()))
		{}

		template<typename T, size_t N>
		const_byte_array_view(T(&arr)[N]) : data_(arr), n(N * sizeof(T)) {}

		void const* data() const { return data_; }
		size_t size() const { return n; }
		size_t byte_size() const { return n; }

		operator bool() const { return data_; }
	private:
		byte const* data_;
		size_t n;
	};

	struct byte_array_view {
		byte_array_view() : data_(nullptr), n(0) {}
		byte_array_view(byte* ptr, size_t n) : data_(ptr), n(n) {}

		template<typename Cont>
		byte_array_view(Cont& c) : 
			byte_array_view(reinterpret_cast<byte*>(c.data()), c.size() * sizeof(*c.data())) 
		{}

		byte* data() const { return data_; }
		size_t size() const { return n; }
		size_t byte_size() const { return n; }

		operator bool() const { return data_; }
		operator const_byte_array_view() const { return const_byte_array_view(data(), byte_size()); }
	private:
		byte* data_;
		size_t n;
	};
}}}
