
CMAKE_MINIMUM_REQUIRED(VERSION 3.11)

PROJECT(FwJson)

OPTION(BUILD_TESTS "Build tests" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(FwJson_SOURCES
	src/fwjson.cpp
)

set(FwJson_HEADERS
)

set(FwJson_PUBLIC
	include/fwjson/fwjson.h
)

########################################
# Target

include_directories(
	${PROJECT_SOURCE_DIR}/include
)

set(FwJson_LIBRARY fwjson)
add_library(${FwJson_LIBRARY} STATIC
	${FwJson_SOURCES}
	${FwJson_HEADERS}
	${FwJson_PUBLIC}
)

set_target_properties(${FwJson_LIBRARY} PROPERTIES FOLDER Libs)
set(FwJson_LIBRARY ${FwJson_LIBRARY} PARENT_SCOPE)
set(FwJson_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/include PARENT_SCOPE)

if(BUILD_TESTS)
	add_subdirectory(tests)
endif ()

MESSAGE(STATUS "FwJson library added")
