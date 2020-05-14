add_library(world SHARED
	errors.cpp
	errors.h
	level.cpp
	level.h
	level_loader.cpp
	level_loader.h
	tile.cpp
	tile_descriptor.cpp
	tile_descriptor.h
	tile.h
	tile_loader.cpp
	tile_loader.h
)

target_link_libraries(world
	PUBLIC
		core_error
		glm	
		easyloggingpp
		utility
		Boost::boost
		SOIL
		rendering
)
