#pragma once
#include <enum.h>
#include <system_error>
#include <cassert>


namespace eversim { namespace core {

	struct source_location {
		int line;
		char const* file;
		char const* function = nullptr;
	};

#define GET_SOURCE_LOCATION() ::eversim::core::source_location{__LINE__, __FILE__}
#define GET_SOURCE_LOCATION_F() ::eversim::core::source_location{__LINE__, __FILE__, __func__}


	class eversim_error : public std::system_error {
	public:
		eversim_error(std::error_code errc, source_location const& loc);
		eversim_error(std::error_code errc, std::string const& message, source_location const& loc);
		eversim_error(std::error_code errc, char const* message, source_location const& loc);

		char const* what() const override;
	private:
		source_location loc;
		std::string message;
	};

	BETTER_ENUM(generic_error, uint8_t, \
		AssertionFailure = 1,			\
		FileNotFound,					\
		InvalidArgument,				\
		InvalidEnum						\
	)

	// Because BETTER_ENUM uses an enum embedded into a type, a direct conversion doesn't work.
	// so we need to take this embedded enum as argument
	std::error_code make_error_code(generic_error::_enumerated);

}
}

#define THROW_IMPL_GET_MACRO(_1,_2,NAME,...) NAME

#define EVERSIM_THROW_MESSAGE(errc, message) \
	throw ::eversim::core::eversim_error(errc, message, GET_SOURCE_LOCATION_F())

#define EVERSIM_THROW_SIMPLE(errc) \
	throw ::eversim::core::eversim_error(errc, GET_SOURCE_LOCATION_F())

#define EVERSIM_THROW(...) THROW_IMPL_GET_MACRO(__VA_ARGS__, EVERSIM_THROW_MESSAGE, EVERSIM_THROW_SIMPLE, FOO)(__VA_ARGS__)

#define EVERSIM_ASSERT(cond) assert(cond)

namespace std {
	template<>
	struct is_error_code_enum<eversim::core::generic_error::_enumerated> : true_type {};
}