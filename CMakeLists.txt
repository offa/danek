cmake_minimum_required(VERSION 3.2)

project(danek VERSION 0.0.4)
message(STATUS "~~~ ${PROJECT_NAME} ${PROJECT_VERSION} ~~~")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(Options)
include(GNUInstallDirs)
include(Compiler)
include(CppStd)
include(ClangFormat)
include(Templates)

if( COVERAGE )
    include(Coverage)
endif()

if( ENABLE_CPPCHECK )
    include(CppCheck)
endif()

add_definitions(-Wall -Wextra -pedantic -Werror -Wshadow -Wold-style-cast)

include_directories("include")

subdirs(src)


if( UNIT_TESTS )
    enable_testing()
    subdirs(test)
endif()

if( BUILD_SCHEMA_TESTS )
    subdirs(tests/schema-types)
endif()

if( BUILD_DEMOS )
    subdirs(demos)
endif()
