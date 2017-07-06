
if( CMAKE_BUILD_TYPE )
    message(STATUS "Build Type : ${CMAKE_BUILD_TYPE}")
else()
    message(STATUS "Build Type : None")
endif()


macro(_print_option _option _name)
    message(STATUS "${_name} : ${${_option}}")
endmacro()



option(UNIT_TESTS "Build Unit Tests" ON)
_print_option(UNIT_TESTS "Unit Tests")

option(COVERAGE "Enable Coverage" OFF)
_print_option(COVERAGE "Coverage")

option(BUILD_TOOLS "Build the tools" ON)
_print_option(BUILD_TOOLS "Build Tools")

option(BUILD_SCHEMA_TESTS "Build the Schema Tests" OFF)
_print_option(BUILD_SCHEMA_TESTS "Build Schema Tests")

option(BUILD_DEMOS "Build the Demos" OFF)
_print_option(BUILD_DEMOS "Build Demos")

option(BUILD_SHARED_LIBS "Build Shared Library" OFF)
_print_option(BUILD_SHARED_LIBS "Build shared library")

option(ENABLE_CPPCHECK "Enables CppCheck (Requires Compile Commands)" OFF)
_print_option(ENABLE_CPPCHECK "Enable CppCheck")
