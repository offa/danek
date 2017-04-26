cmake_minimum_required(VERSION 3.2)

project(danek VERSION 0.0.1)
message(STATUS "~~~ ${PROJECT_NAME} ${PROJECT_VERSION} ~~~")

option(BUILD_SCHEMA_TESTS "Build Schema tests." OFF)
option(BUILD_SHARED_LIBS "Build shared library" OFF)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(GNUInstallDirs)
include(Compiler)
include(CppStd)


add_definitions(-Wall -Wextra -pedantic -Werror -Wshadow)

include_directories("include")

subdirs(src)

if( BUILD_SCHEMA_TESTS )
    subdirs(tests/schema-types)
endif()
