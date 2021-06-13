# Debug, Release, RelWithDebInfo
# cmake ..
# cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug ..
# cmake --build . -- VERBOSE=1
# ctest -V

cmake_minimum_required(VERSION 3.12)
project(test2)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(zdebug $<$<CONFIG:Debug>:-DZDEBUG>)

list(APPEND flags "-Wall" "-Wextra" "-Wpedantic" "-m64" ${zdebug})
# list(APPEND flags "-m64")
# list(APPEND flags "-Werror")

add_executable(main main.cpp)

target_compile_options(main
  PRIVATE
    ${flags}
  )

set_target_properties(main
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  )

add_test(NAME test1 COMMAND main)
