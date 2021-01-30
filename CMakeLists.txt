cmake_minimum_required(VERSION 3.8)

project(TTFRRW_App)

option(TTFRRW_GENERATE_TEST_APP "TTFRRW : Generate test app" OFF)
option(TTFRRW_USE_PROFILER_TRACY "TTFRRW : Enable Tracy Profiler" OFF)

#############################################################################
## TRACY
#############################################################################
#############################################################################
if (TTFRRW_USE_PROFILER_TRACY)
	if (NOT TRACY_LIBRARIES)
		set(TRACY_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/tracy)
		file(GLOB TRACY_SOURCES ${TRACY_INCLUDE_DIR}/TracyClient.cpp)
		add_library(tracy STATIC ${TRACY_SOURCES})
		set_target_properties(tracy PROPERTIES LINKER_LANGUAGE CXX)
		set_target_properties(tracy PROPERTIES FOLDER 3rdparty)
		set(TRACY_LIBRARIES tracy)
		if(NOT WIN32)
			set(TRACY_LIBRARIES ${TRACY_LIBRARIES} dl pthread)
		endif()
	endif()
	add_definitions(-DTRACY_ENABLE)
endif()
#############################################################################

enable_language(C CXX)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
add_definitions(-DPROJECT_PATH="${CMAKE_SOURCE_DIR}")

file(GLOB SRC 
	${CMAKE_CURRENT_SOURCE_DIR}/ttfrrw.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/ttfrrw.h)
	
if(UNIX)
	if(APPLE)
		add_definitions(-DAPPLE)
	else()
		add_definitions(-DLINUX)
	endif()
	add_definitions(-DUNIX)
elseif(WIN32)
	add_definitions(-DWIN32)
	if(MINGW)
		add_definitions(-DMINGW32)
		add_definitions(-DHAVE_STRTOK_R)
		add_definitions(-DHAVE___BUILTIN_FFS)
	endif()
endif()

if(MSVC)
    # Ignore 4055 for glad
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /IGNORE:4055")
	add_definitions(-D__STDC__)
	add_definitions(-DMSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11") ## -std=gnu++0x
endif ()

add_library(ttfrrw STATIC ${SRC})
target_link_libraries(ttfrrw ${TRACY_LIBRARIES})

include_directories(
	.
	${TRACY_INCLUDE_DIR})

set_target_properties(ttfrrw PROPERTIES LINKER_LANGUAGE CXX)

set(TTFRRW_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set(TTFRRW_LIBRARIES ttfrrw PARENT_SCOPE)
set(TTFRRW_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)

if (TTFRRW_GENERATE_TEST_APP)
add_executable(TTFRRW_App main.cpp)
target_link_libraries(TTFRRW_App ttfrrw ${TRACY_LIBRARIES})
set_property(TARGET TTFRRW_App PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()