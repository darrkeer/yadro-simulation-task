cmake_minimum_required(VERSION 3.16)
project(yadro-simulation-task)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_library(event-parser STATIC src/event_parser.cpp src/event_parser.h)
add_library(events STATIC ./src/events.cpp ./src/events.h)
add_library(state-utils STATIC ./src/state-utils.cpp ./src/state-utils.h)

add_executable(task ./src/main.cpp)
add_executable(tests tests/parser-tests.cpp)

target_link_libraries(events state-utils)
target_link_libraries(event-parser events)
target_link_libraries(task event-parser)

target_link_libraries(tests GTest::gtest_main event-parser)

include(GoogleTest)
gtest_discover_tests(tests)