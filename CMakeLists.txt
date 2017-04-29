cmake_minimum_required(VERSION 3.2)

project(danek VERSION 0.0.1)
message(STATUS "~~~ ${PROJECT_NAME} ${PROJECT_VERSION} ~~~")

option(UNIT_TESTS "Build Unit Tests" ON)
option(COVERAGE "Enable Coverage" OFF)
option(SCHEMA_TESTS "Build Schema tests." OFF)
option(BUILD_SHARED_LIBS "Build shared library" OFF)
option(DEMOS "Build demos." OFF)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(GNUInstallDirs)
include(Compiler)
include(CppStd)

if( COVERAGE )
    include(Coverage)
endif()

add_definitions(-Wall -Wextra -pedantic -Werror -Wshadow)

include_directories("include")

subdirs(src)


if( UNIT_TESTS )
    subdirs(test)
endif()

if( SCHEMA_TESTS )
    subdirs(tests/schema-types)
endif()

if( DEMOS )
    subdirs(demos)
endif()
