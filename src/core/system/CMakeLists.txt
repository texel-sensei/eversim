add_library(system SHARED
	component.cpp
	component.h
	errors.cpp
	errors.h
	game.cpp
	game.h
	gameobject.cpp
	gameobject.h

	components/physics_component.cpp
	components/physics_component.h
	components/rendering_component.cpp
	components/rendering_component.h
)

target_link_libraries(system
	PUBLIC
		core_error
		eversim_common
		utility
		physics
		easyloggingpp
		Boost::boost
		glm
		rendering
)
