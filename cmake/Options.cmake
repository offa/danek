
if( CMAKE_BUILD_TYPE )
    message(STATUS "Build Type : ${CMAKE_BUILD_TYPE}")
else()
    message(STATUS "Build Type : None")
endif()


macro(_print_option _option _name _value _docstring)
    option(${_option} "${_docstring}" ${${_value}})
    message(STATUS "${_name} : ${${_option}}")
endmacro()


_print_option(UNIT_TESTS "Unit Tests" ON "Build Unit Tests")
_print_option(COVERAGE "Coverage" OFF "Enable Coverage")
_print_option(BUILD_TOOLS "Build Tools" ON "Build the tools")
_print_option(BUILD_SCHEMA_TESTS "Build Schema Tests" OFF "Build the Schema Tests")
_print_option(BUILD_DEMOS "Build Demos" OFF "Build the Demos")
_print_option(BUILD_SHARED_LIBS "Build shared library" OFF "Build Shared Library")

