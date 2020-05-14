#pragma once

#include "core/utility/time/clock.h"

namespace eversim { namespace core { namespace utility {

	class controllable_clock final : public clock {
	public:
		controllable_clock() = default;
		explicit controllable_clock(time_point begin) : current_time(begin){}

		void advance(duration dur)
		{
			current_time += dur;
		}

	protected:
		time_point get_time() const noexcept override
		{
			return current_time;
		}
	private:
		time_point current_time;
	};

}}}
