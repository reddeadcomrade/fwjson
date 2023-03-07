cmake_minimum_required (VERSION 3.22)

project(FwJson_tests)

include(FetchContent)

set(FETCHCONTENT_QUIET OFF)

if (NOT "$ENV{GTEST_PRE_BUILD_URL}" STREQUAL "")
	set(GTEST_PRE_BUILD_URL "$ENV{GTEST_PRE_BUILD_URL}")
	set(GTEST_ROOT_DIR "${CMAKE_CURRENT_BINARY_DIR}/googletest")

	message(STATUS "Fetch googletest prebuild from ${GTEST_PRE_BUILD_URL}")

	FetchContent_Declare(
		googletest
		URL ${GTEST_PRE_BUILD_URL}
		SOURCE_DIR ${GTEST_ROOT_DIR}
	)
	FetchContent_MakeAvailable(googletest)

	include(${GTEST_ROOT_DIR}/lib/cmake/GTest/GTestConfig.cmake)
else ()
	FetchContent_Declare(
		googletest
		GIT_REPOSITORY https://github.com/google/googletest
		GIT_TAG        release-1.12.1
	)
	FetchContent_MakeAvailable(googletest)
endif ()

if(NOT TARGET GTest::gtest)
	message(FATAL_ERROR "Target GTest::gtest not available.")
endif()

set(SOURCES_TESTCASES
	test_CharIterator.cpp
	test_Json.cpp
	test_Strings.cpp
	test_Parsing.cpp
)

set(BIN FwJson_tests)

add_executable(${BIN} ${SOURCES_TESTCASES})

target_link_libraries(${BIN}
	${FwJson_LIBRARY}
	GTest::gtest
	GTest::gtest_main
)
set_target_properties(${BIN} PROPERTIES FOLDER Test)
