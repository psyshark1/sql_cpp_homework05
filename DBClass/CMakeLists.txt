cmake_minimum_required (VERSION 3.8)

project(dbclass)

add_subdirectory("C:/Users/user/source/repos/libpqxx-7.8.1" libpqxx-build)

include_directories(DBClass)
include_directories(../Exception)

set(DBCLASS DBClass.h DBClass.cpp ../Exception/ExceptDBClass.h ../Exception/ExceptDBClass.cpp)

add_library(dbclass SHARED ${DBCLASS})

target_compile_definitions(dbclass PRIVATE EX_DBC)

target_compile_features(dbclass PRIVATE cxx_std_17)
target_link_libraries(dbclass pqxx)