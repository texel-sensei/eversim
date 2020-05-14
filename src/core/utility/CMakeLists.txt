# general utility lib

add_library(utility SHARED
	# One file needs to include the INITIALIZE_EASYLOGGINGPP
	# macro, this is the following one:
	logger.cpp

	area.cpp
	area.h
	array_view.h
	byte_array_view.h
	event_delegate.h
	filesystem_wrapper.h
	helper.h
	object_pool.h
	plattform.h
	resource_manager.h
	sdl.h
	spatial_hashmap.h
	stacktrace.cpp
	stacktrace.h

	time/clock.h
	time/controllable_clock.h
	time/print.h
	time/realtime_clock.h
	time/scoped_timer.cpp
	time/scoped_timer.h

	math/angular.h
	math/constants.h
	math/line.cpp
	math/line.h
	math/matrix_helper.h
)

target_link_libraries(utility
	PUBLIC
		glm
		OpenGL::GL
		GLEW::GLEW
		SDL2::SDL2
		Boost::boost
		dl
		easyloggingpp
)
