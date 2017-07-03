#pragma once
#include <glm/glm.hpp>
#include <utility>
#include <sstream>

namespace eversim { namespace core { namespace utility {
	namespace detail{
		template<typename Func>
		struct finally_helper {
			finally_helper(Func&& f) : f(std::forward<Func>(f)) {}
			~finally_helper() noexcept(noexcept(finally_helper::f())) {
				f();
			}
		private:
			Func f;
		};
	} // namespace detail

	template<typename Func>
	auto finally(Func&& f) -> detail::finally_helper<Func> {
		return {std::forward<Func>(f)};	
	}

	template<typename T>
	struct print_type;

	template<typename F, typename... Args>
	struct is_callable{
		template<typename U> static auto test(U* p) -> decltype((*p)(std::declval<Args>()...), void(), std::true_type());
		template<typename U> static auto test(...) -> decltype(std::false_type());

		using type = decltype(test<F>(nullptr));
		static constexpr bool value = type::value;
	};

	template<typename T>
	std::string to_string(T const& t) {
		std::stringstream ss;
		ss << t;
		return ss.str();
	}

} /* namespace utility */ } /* namespace core */ } /* namespace eversim */

namespace glm {
	template<typename T>
	std::ostream& operator<<(std::ostream& os, glm::tvec2<T> const& v) {
		return os << "["<<v.x<<", "<<v.y<<"]";
	}
	template<typename T>
	std::ostream& operator<<(std::ostream& os, glm::tvec3<T> const& v) {
		return os << "["<<v.x<<", "<<v.y<<", "<<v.z<<"]";
	}
	template<typename T>
	std::ostream& operator<<(std::ostream& os, glm::tvec4<T> const& v) {
		return os << "["<<v.x<<", "<<v.y<<", "<<v.z << ", " << v.w <<"]";
	}
} /* glm */
