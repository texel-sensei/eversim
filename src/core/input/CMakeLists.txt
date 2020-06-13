add_library(input SHARED
	contextloader.cpp
	contextloader.h
	devicehandler.cpp
	devicehandler.h
	inputconstants.h
	inputcontext.cpp
	inputcontext.h
	inputdevice.h
	inputevent.cpp
	inputevent.h
	inputhandler.cpp
	inputhandler.h
	jsonloader.h
	rawinputconstants.h
)

target_link_libraries(input
	PUBLIC
		core_error
		eversim_common
		utility
		easyloggingpp
		Boost::boost
		SDL2::SDL2
		glm
)
