cmake_minimum_required(VERSION 3.16)
project(fun_client)

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    # add_compile_options(-fstandalone-debug)             # uncomment this if you are using clang, and clion debugger cannot show value of std::string
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
else ()
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO)
endif ()

include(FetchContent)

FetchContent_Declare(cli11 URL https://github.com/CLIUtils/CLI11/archive/refs/tags/v2.1.2.zip)
FetchContent_MakeAvailable(cli11)

FetchContent_Declare(catch2 URL https://github.com/catchorg/Catch2/archive/refs/tags/v2.13.7.zip)
FetchContent_MakeAvailable(catch2)
INCLUDE(${Catch2_SOURCE_DIR}/contrib/Catch.cmake)

FetchContent_Declare(spdlog URL https://github.com/gabime/spdlog/archive/v1.8.1.tar.gz)
FetchContent_MakeAvailable(spdlog)

add_library(transport STATIC
        src/tcp_socket.cpp
        src/unix_socket.cpp
        src/fun_client.cpp
        src/matching_engine.cpp
        src/message_parser.cpp
        src/feed_player.cpp
        src/final.cpp
        src/timer.cpp
        src/feed_player_recorder.cpp)
target_include_directories(transport PUBLIC ./include)
target_link_libraries(transport PUBLIC
        spdlog)

add_executable(fun-client src/main.cpp)
target_link_libraries(fun-client PUBLIC
        transport
        CLI11::CLI11)

add_executable(matching-engine src/main_matching_engine.cpp)
target_link_libraries(matching-engine PUBLIC
        transport
        CLI11::CLI11)

add_subdirectory(tests)