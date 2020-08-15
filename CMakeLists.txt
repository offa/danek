cmake_minimum_required(VERSION 3.10)

project(danek VERSION 0.0.6)
message(STATUS "~~~ ${PROJECT_NAME} ${PROJECT_VERSION} ~~~")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(Options)
include(GNUInstallDirs)
include(Compiler)
include(ClangFormat)

if( COVERAGE )
    include(Coverage)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall
                    -Wextra
                    -pedantic
                    -Werror
                    -Wshadow
                    -Wold-style-cast
                    )
add_compile_options_if_supported(-Wduplicated-cond
                                -Wduplicated-branches
                                -Wlogical-op
                                -Wnull-dereference
                                -Wuseless-cast
                                )

include(Templates)

include_directories("include")
add_subdirectory(src)


if( UNIT_TESTS )
    enable_testing()
    add_subdirectory(test)
endif()

if( BUILD_SCHEMA_TESTS )
    add_subdirectory(tests/schema-types)
endif()

if( BUILD_DEMOS )
    add_subdirectory(demos)
endif()
