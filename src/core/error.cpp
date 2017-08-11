#include "core/error.h"
#include <string>
#include <sstream>

using namespace std;
using namespace literals;

namespace eversim {namespace core {

	eversim_error::eversim_error(error_code errc, source_location const& loc)
		: eversim_error(errc, nullptr,  loc)
	{
	}

	eversim_error::eversim_error(error_code errc, string const& msg, source_location const& loc):
		eversim_error(
			errc, 
			(msg.empty()) ? nullptr : msg.c_str(),
			loc
		)
	{}

	eversim_error::eversim_error(std::error_code errc, char const* msg, source_location const& loc)
		: system_error(errc), loc(loc)
	{
		ostringstream ss;
		ss << loc.file << ':' << loc.line;

		if (loc.function)
		{
			ss << "(" << loc.function << ")";
		}

		ss << "[" << errc.category().name() << ": " << errc.message() << "]";

		if (msg)
		{
			ss << " " << msg;
		}

		message = ss.str();
	}

	char const* eversim_error::what() const
	{
		return message.c_str();
	}

	namespace {
		struct eversim_error_category : error_category {

			const char* name() const noexcept override
			{
				return "general";
			}

			string message(int _Errval) const override
			{
				switch (static_cast<generic_error>(_Errval))
				{
				case generic_error::AssertionFailure:
					return "assertion failed";
				case generic_error::FileNotFound:
					return "file not found";
				case generic_error::InvalidArgument: 
					return "Invalid argument";
				case generic_error::InvalidEnum: 
					return "Invalid enum value";
				default:
					return "(unknown)";
				}
			}
		};

		const eversim_error_category general_error;
	}

	error_code make_error_code(generic_error err)
	{
		return { static_cast<int>(err), general_error };
	}
}}
