cmake_minimum_required(VERSION 3.1)

project(TTFRRW_App)

enable_language(C CXX)

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

include_directories(.)

set_target_properties(ttfrrw PROPERTIES LINKER_LANGUAGE CXX)

set(TTFRRW_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set(TTFRRW_LIBRARIES ttfrrw PARENT_SCOPE)
set(TTFRRW_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)

add_executable(TTFRRW_App main.cpp)
target_link_libraries(TTFRRW_App ttfrrw)