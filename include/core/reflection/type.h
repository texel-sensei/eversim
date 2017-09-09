#pragma once

#include <type_traits>
#include <boost/type_index.hpp>
#include <typeindex>


namespace eversim::core::reflection {
	
	template<typename T>
	struct Type {
		using type = T;

		constexpr Type() = default;
		Type& operator=(const Type&) = delete;

		constexpr auto name() const
		{
			return boost::typeindex::type_id_with_cvr<type>().pretty_name();
		}

		constexpr auto type_index() const
		{
			return std::type_index(typeid(type));
		}

		template<typename U>
		constexpr bool operator==(Type<U> const&) const
		{
			return std::is_same_v<T, U>;
		}

		template<typename U>
		constexpr bool operator!=(Type<U> const& other) const
		{
			return !(*this == other);
		}
	};

	template<typename T>
	constexpr Type<T> type_c{};


} // namespace eversim::core::utility