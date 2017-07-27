#include <easylogging++.h>
INITIALIZE_EASYLOGGINGPP

// easylogging++ provides it's own set of CHECK macros.
//Those clash with the catch.hpp macros.
#undef CHECK

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>