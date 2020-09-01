#include <easylogging++.h>
INITIALIZE_EASYLOGGINGPP

// easylogging++ provides it's own set of CHECK macros.
//Those clash with the catch.hpp macros.
#undef CHECK

#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

int main(int argc, char* argv[])
{
	START_EASYLOGGINGPP(argc, argv);
	el::Configurations default_config;
	default_config.setToDefault();
	default_config.setGlobally(el::ConfigurationType::Filename, "logs/test_log.txt");
	el::Loggers::reconfigureAllLoggers(default_config);

	int result = Catch::Session().run(argc, argv);

	// global clean-up...

	return (result < 0xff ? result : 0xff);
}
