#pragma once

#include <chrono>
#include <sstream>

namespace eversim {
	namespace core {
		namespace utility {

			template<typename Stream, typename Rep, typename Period>
			Stream& operator<< (Stream& os, std::chrono::duration<Rep, Period> dur) {
				using namespace std;
				using namespace chrono;
				auto h = duration_cast<hours>(dur);
				auto min = duration_cast<minutes>(dur -= h);
				auto s = duration_cast<seconds>(dur -= min);
				auto ms = duration_cast<milliseconds>(dur -= s);
				auto us = duration_cast<microseconds>(dur -= ms);
				auto print = false;
				if (h.count() > 0) { os << h.count() << "h"; print = true; }
				if (min.count() > 0) { os << (print ? " " : "") << min.count() << "min"; print = true; }
				if (s.count() > 0) { os << (print ? " " : "") << s.count() << "s"; print = true; }
				if (ms.count() > 0) { os << (print ? " " : "") << ms.count() << "ms"; print = true; }
				if (us.count() > 0) { os << (print ? " " : "") << us.count() << "us"; print = true; }
				if (!print) {
					os << "~0";
				}
				return os;
			}

			template<typename Rep, typename Period>
			std::string to_string(std::chrono::duration<Rep, Period> const& dur) {
				std::ostringstream ss;
				ss << dur;
				return ss.str();
			}
		}
	}
}
		