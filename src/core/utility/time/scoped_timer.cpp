#include "core/utility/time/scoped_timer.h"
#include "core/utility/time/print.h"
#include <easylogging++.h>
#include <chrono>

using namespace std;

namespace eversim { namespace core { namespace utility {

	void log_time_reporter::operator()(clock::duration dur) const noexcept
	{
		LOG(INFO) << message << " [" << to_string(dur) << "]";
	}

	scoped_timer::scoped_timer(string const& message, shared_ptr<utility::clock> clk)
		: scoped_timer(clk, log_time_reporter{message})
	{
	}

	scoped_timer::scoped_timer(reporter_function rep)
		: scoped_timer(make_shared<realtime_clock>(), rep)
	{
	}

	scoped_timer::scoped_timer(shared_ptr<utility::clock> clk, reporter_function rep)
		: clock(move(clk)), starting_time(clock->now()), reporter(move(rep))
	{
	}

	scoped_timer::~scoped_timer()
	{
		const auto end_time = clock->now();
		const auto duration = end_time - starting_time;
		reporter(duration);
	}
}}}
