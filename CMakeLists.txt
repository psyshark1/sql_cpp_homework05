cmake_minimum_required (VERSION 3.8)

project (libpq)

include_directories(DBClass)
include_directories(Exception)

add_executable (libpq libpq.cpp Exception/ExceptDBClass.h Exception/ExceptDBClass.cpp)

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

add_subdirectory(DBClass)

target_compile_features(libpq PRIVATE cxx_std_17)
target_link_libraries(libpq dbclass)
