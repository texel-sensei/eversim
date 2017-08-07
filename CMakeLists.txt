cmake_minimum_required(VERSION 2.8)

set(EXTERNALS ${CMAKE_CURRENT_SOURCE_DIR}/external)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

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

#dear imgui
file(GLOB_RECURSE LODEPNG_SOURCES ${EXTERNALS}/lodepng/src/*.cpp)
add_library(lodepng ${LODEPNG_SOURCES})
target_include_directories(lodepng PUBLIC ${EXTERNALS}/lodepng/include/lodepng)
include_directories(${EXTERNALS}/lodepng/include/)
SET(LIBS ${LIBS} lodepng)

#soil
add_subdirectory(${EXTERNALS}/soil)
include_directories(${EXTERNALS}/soil/include/)
SET(LIBS ${LIBS} SOIL)

#catch
set(CATCH_INCLUDE_DIR ${EXTERNALS}/catch)
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

#boost
set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.64.0)
include_directories(${Boost_INCLUDE_DIRS})

#boost poly collection
include_directories(${EXTERNALS}/poly_collection/include)

#easyloggingpp
option(build_static_lib "Build easyloggingpp as a static library" ON)
add_definitions(-DELPP_FEATURE_ALL)
add_subdirectory(${EXTERNALS}/easyloggingpp)
set(CMAKE_CXX_STANDARD 14) # required because easyloggingpp sets standard to 11
include_directories(${EXTERNALS}/easyloggingpp/src)
SET(LIBS ${LIBS} easyloggingpp)

############# COMPILER FLAGS ###############

if(WIN32)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP") #multiprocessor compilation for visual studio
endif(WIN32)

add_definitions(-DNOMINMAX)

############# SOURCE ###############

file(GLOB_RECURSE CORE_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/core/*.h)
file(GLOB_RECURSE GAME_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/game/*.h)
file(GLOB_RECURSE EDITOR_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/editor/*.h)


file(GLOB_RECURSE CORE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cpp)
file(GLOB_RECURSE GAME_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/game/*.cpp)
file(GLOB_RECURSE EDITOR_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/editor/*.cpp)

# core source groups
source_group("utility" REGULAR_EXPRESSION /utility/)
source_group("utility\\time" REGULAR_EXPRESSION /utility/time/)
source_group("rendering" REGULAR_EXPRESSION /rendering/)
source_group("physics" REGULAR_EXPRESSION /physics/)
source_group("physics\\constraints" REGULAR_EXPRESSION /physics/constraints/)
source_group("world" REGULAR_EXPRESSION /world/)
source_group("system" REGULAR_EXPRESSION /system/)
source_group("system\\imgui" REGULAR_EXPRESSION /system/imgui/)

#editor source groups
source_group("imgui" REGULAR_EXPRESSION /editor/imgui/)
source_group("core" REGULAR_EXPRESSION /editor/core/)
source_group("windows" REGULAR_EXPRESSION /editor/windows/)

add_library(eversim STATIC ${CORE_HEADERS} ${CORE_SOURCES})
add_library(editor STATIC ${EDITOR_HEADERS} ${EDITOR_SOURCES})
add_executable(game ${GAME_HEADERS} ${GAME_SOURCES})


target_link_libraries(eversim ${LIBS})
target_link_libraries(editor eversim)
target_link_libraries(game editor eversim)

#######################################################################
#                             testing                                 #
#######################################################################

set(TEST_BIN eversim_tests)
file(GLOB_RECURSE TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp)
add_executable(${TEST_BIN} ${TEST_SOURCES})
target_link_libraries(${TEST_BIN} Catch ${LIBS} eversim)

enable_testing()
include(ParseAndAddCatchTests)
ParseAndAddCatchTests(${TEST_BIN})
