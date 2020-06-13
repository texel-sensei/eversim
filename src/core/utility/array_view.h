#pragma once

#include "core/error.h"

#include <cstdint>
#include <type_traits>
#include <cassert>

#include <span>

namespace eversim { namespace core { namespace utility {

	template<typename T>
	using array_view = std::span<T>;

	template <typename T>
	array_view<T> make_array_view(T* data_, size_t n)
	{
		return array_view<T>(data_, n);
	}

	template <typename T, size_t N>
	array_view<T> make_array_view(T (&arr)[N])
	{
		return array_view<T>(arr);
	}

	template <typename Container>
	auto make_array_view(Container&& cont)
	{
		using type = typename std::remove_reference<decltype(*cont.data())>::type;
		return array_view<type>(cont);
	}
}}}
