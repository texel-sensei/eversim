# Main eversim library

add_library(core_error SHARED
	error.h	
	error.cpp
)

set(modules 
	utility 
	system 
	physics 
	rendering 
	world 
	input
)

# Add all library modules
foreach(module ${modules})
	add_subdirectory(${module})
endforeach()

add_library(eversim INTERFACE)
target_link_libraries(eversim INTERFACE
	${modules}
)

	

