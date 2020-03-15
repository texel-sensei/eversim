#pragma once

#include "core/utility/time/realtime_clock.h"
#include "core/utility/time/clock.h"

#include <memory>
#include <functional>
#include <string>

namespace eversim {namespace core{namespace utility {
	
	struct log_time_reporter {
		std::string message;
		void operator()(clock::duration) const noexcept;
	};

	class scoped_timer {
	public:
		using reporter_function = std::function<void(clock::duration)>;
		
		explicit scoped_timer(std::string const& message, std::shared_ptr<clock> clk = std::make_shared<realtime_clock>());
		explicit scoped_timer(reporter_function rep);
		scoped_timer(std::shared_ptr<clock> clk, reporter_function rep);

		~scoped_timer();

		scoped_timer(scoped_timer const&) = delete;
	private:
		std::shared_ptr<clock> clock_;
		const clock::time_point starting_time;

		reporter_function reporter;
	};

} } }
