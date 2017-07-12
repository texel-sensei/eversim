#pragma once

#include <cstdint>
#include <type_traits>
#include <cassert>

namespace eversim { namespace core { namespace utility {

	template <typename T>
	struct array_view {
		using value_type = T;
		using reference = value_type&;
		using const_reference = value_type const&;
		using pointer = value_type*;
		using const_pointer = value_type const*;
		using iterator = pointer;
		using const_iterator = const_pointer;
		using size_type = size_t;
		using difference_type = ptrdiff_t;

		array_view() : data_(nullptr), n(0)
		{
		}

		array_view(T* data_, size_t n) : data_(data_), n(n)
		{
		}

		array_view(array_view<T> const&) = default;
		array_view& operator=(array_view<T> const&) = default;

		template <size_t N>
		array_view(T (&arr)[N]) : data_(arr), n(N)
		{
		}

		template <typename Cont,
		          typename = typename std::enable_if<
			          std::is_convertible<decltype(std::declval<Cont>().data()), T*>::value
			          && !std::is_same<Cont, array_view<T>>::value
		          >
		          ::type
		>
		array_view(Cont& dv) : data_(dv.data()), n(dv.size())
		{
		}

		reference operator[](size_t id) const noexcept
		{
			assert(id < size());
			return data_[id];
		}

		reference back() noexcept
		{
			assert(size() > 0);
			return data_[n - 1];
		}

		const_reference back() const noexcept
		{
			assert(size() > 0);
			return data_[n - 1];
		}

		pointer data() const noexcept
		{
			return data_;
		}

		size_type size() const noexcept
		{
			return n;
		}

		size_type byte_size() const noexcept
		{
			return sizeof(T) * n;
		}

		iterator begin() const noexcept
		{
			return data_;
		}

		iterator end() const noexcept
		{
			return data_ + n;
		}

		array_view<T> slice(size_t left, size_t right) const
		{
			return array_view<T>(data_ + left, n - right);
		}

		array_view<T> slice_n(size_t offset, size_t n) const
		{
			return array_view<T>(data_ + offset, n);
		}

		operator bool() const noexcept
		{
			return data_;
		}

		operator array_view<const T>()
		{
			return array_view<const
				T>(data_, n);
		}

	private:
		T* data_;
		size_t n;
	};

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
