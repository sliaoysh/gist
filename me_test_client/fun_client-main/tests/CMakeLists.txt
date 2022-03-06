add_library(catch-main STATIC
        catch_main.cpp)
target_link_libraries(catch-main
        Catch2::Catch2
        transport)

add_executable(unit-tests
        unit_tests.cpp)
target_link_libraries(unit-tests
        catch-main
        transport)

include(CTest)
catch_discover_tests(unit-tests)
