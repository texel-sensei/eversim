#pragma once

#include <chrono>

namespace eversim {namespace core{ namespace utility {
	
	class clock {
	public:
		using duration					= std::chrono::milliseconds;
		using rep						= duration::rep;
		using period					= duration::period;
		using time_point				= std::chrono::time_point<clock>;
		static constexpr bool is_steady	= false;

		virtual ~clock(){}
		time_point now() const noexcept {
			return get_time();
		}

	protected:
		virtual time_point get_time() const noexcept = 0;
	};

} } }