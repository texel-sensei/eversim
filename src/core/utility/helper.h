#pragma once
#include <glm/glm.hpp>
#include <functional>
#include <utility>
#include <sstream>

namespace eversim { namespace core { namespace utility {
	
	template<typename F, typename... Args>
	struct is_callable {
		template<typename U> static auto test(U* p) -> decltype((*p)(std::declval<Args>()...), void(), std::true_type());
		template<typename U> static auto test(...) -> decltype(std::false_type());

		using type = decltype(test<F>(nullptr));
		static constexpr bool value = type::value;
	};

	template<typename F, typename... Args>
	constexpr bool is_callable_v = is_callable<F, Args...>::value;
		
	struct final_action {
		final_action() = default;
		template<typename Func>
		final_action(Func&& f) : f(std::forward<Func>(f)) {}
		~final_action() {
			if(f) f();
		}
		final_action(final_action&&) = default;
		
		final_action& operator=(final_action const&) = delete;
		final_action& operator=(final_action&&) = default;

		template<typename Func, 
			typename = std::enable_if_t<is_callable_v<Func>>
		>
		final_action& operator=(Func&& new_func) {
			if (f) f();
			f = std::forward<Func>(new_func);
			return *this;
		}
	private:
		std::function<void()> f;
	};
	

	template<typename Func>
	auto finally(Func&& f) -> final_action {
		return {std::forward<Func>(f)};	
	}

	template<typename T>
	struct print_type;

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
