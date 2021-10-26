cmake_minimum_required(VERSION 3.20)
project(test3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory(a)

target_include_directories(${PROJECT_NAME} PUBLIC a)

target_link_directories(${PROJECT_NAME} PRIVATE a)

target_link_libraries(${PROJECT_NAME} ladder)
