cmake_minimum_required(VERSION 2.8.11)

project(webcam_capture)

# handle build type
set(CMAKE_BUILD_TYPE Debug CACHE STRING "")

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  message(STATUS "Build type: Debug")
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
  message(STATUS "Build type: Release")
else()
    message(STATUS "Build type: Unknown")
endif()

# enable warnings, C++11
if (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # msvc uses the latest supported C++ by default, so we don't have to set it to C++11
  # force to always compile with W4 warning level
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4 /MP /FS" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /MP /FS")
  endif()
else()
  message(STATUS "Can't set warning level because you are using an unknown compiler. Please consider hacking top-level CMakeLists.txt file to add warnings for your compiler and contributing it to us.")
endif()

# use custom find modules
#set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

# build the library
add_subdirectory(src)

option(TEST_APP "Build the test application" OFF)
if (TEST_APP)
  message(STATUS "Building the test application")
  add_subdirectory(test_app)
else()
  message(STATUS "Skipping the test application")
endif()
