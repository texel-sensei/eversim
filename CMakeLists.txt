cmake_minimum_required(VERSION 2.8)

set(EXTERNALS ${CMAKE_CURRENT_SOURCE_DIR}/external)

set(MAIN_INCLUDE_DIR "" CACHE FILEPATH "Path to your lib includes")
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${MAIN_INCLUDE_DIR}/include)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${MAIN_INCLUDE_DIR}/lib)
include_directories(${MAIN_INCLUDE_DIR}/include)
LINK_DIRECTORIES(${MAIN_INCLUDE_DIR}/lib)

#######################################################################
#                               options                               #
#######################################################################
if(UNIX)
	INCLUDE(FindPkgConfig)
endif(UNIX)

project (eversim)
set (PROGRAM_VERSION_MAJOR 0)
set (PROGRAM_VERSION_MINOR 1)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#######################################################################
#                              Externals                              #
#######################################################################

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)

######### Show all warnings
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /wd4201")
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()

############# Required LIBRARIES ###############

#OPENGL
find_package(OpenGL REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS})
SET(LIBS ${LIBS} ${OPENGL_LIBRARIES})

#SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
SET(LIBS ${LIBS} ${SDL2_LIBRARY})

#GLEW
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS} )
SET(LIBS ${LIBS} ${GLEW_LIBRARIES})

#GLM
find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})
add_definitions(-DGLM_FORCE_RADIANS=1)

#dear imgui
file(GLOB_RECURSE IMGUI_SOURCES ${EXTERNALS}/imgui/src/*.cpp)
add_library(imgui ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ${EXTERNALS}/imgui/include/imgui)
include_directories(${EXTERNALS}/imgui/include/)
SET(LIBS ${LIBS} imgui)

#soil
add_subdirectory(${EXTERNALS}/soil)
include_directories(${EXTERNALS}/soil/include/)

############# COMPILER FLAGS ###############

if(WIN32)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP") #multiprocessor compilation for visual studio
endif(WIN32)

############# SOURCE ###############
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB_RECURSE CORE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cpp)
file(GLOB_RECURSE GAME_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/game/*.cpp)
file(GLOB_RECURSE EDITOR_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/editor/*.cpp)

add_library(eversim STATIC ${CORE_SOURCES})
add_executable(game ${GAME_SOURCES})
add_executable(editor ${EDITOR_SOURCES})

link_libraries(${LIBS})
target_link_libraries(game eversim ${LIBS})
target_link_libraries(editor eversim ${LIBS})

#######################################################################
#                             output dir                              #
#######################################################################

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)