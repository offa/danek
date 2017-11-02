cmake_minimum_required(VERSION 3.8.2)

project(danek VERSION 0.0.5)
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
