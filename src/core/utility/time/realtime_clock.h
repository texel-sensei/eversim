#pragma once

#include "core/utility/time/clock.h"

namespace eversim {namespace core {namespace utility {

	class realtime_clock final : public clock {
	protected:
		virtual time_point get_time() const noexcept override {
			const auto now = std::chrono::steady_clock::now();
			const auto time_passed = std::chrono::duration_cast<duration>(now.time_since_epoch());
			return time_point(time_passed);
		}
	};

}}}