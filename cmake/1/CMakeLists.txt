# cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug ..
# cmake --build .

cmake_minimum_required(VERSION 3.12)

project(test1)

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "***** Using default CMAKE_BUILD_TYPE *****")
  set(CMAKE_BUILD_TYPE Debug)
endif()

message(STATUS "project: ${CMAKE_PROJECT_NAME}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ flags, Debug configuration: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "C++ flags, Release configuration: ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS
        "C++ flags, RelWithDebInfo configuration: ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
message("** C++ compiler flags: ${CMAKE_CXX_FLAGS}")
