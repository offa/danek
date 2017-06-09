
if( CMAKE_BUILD_TYPE )
    message(STATUS "Build Type : ${CMAKE_BUILD_TYPE}")
else()
    message(STATUS "Build Type : None")
endif()


macro(_add_option _option _name _value _docstring)
    option(${_option} "${_docstring}" ${${_value}})
    message(STATUS "${_name} : ${${_option}}")
endmacro()


_add_option(UNIT_TESTS "Unit Tests" ON "Build Unit Tests")
_add_option(COVERAGE "Coverage" OFF "Enable Coverage")
_add_option(BUILD_TOOLS "Build Tools" ON "Build the tools")
_add_option(BUILD_SCHEMA_TESTS "Build Schema Tests" OFF "Build the Schema Tests")
_add_option(BUILD_DEMOS "Build Demos" OFF "Build the Demos")
_add_option(BUILD_SHARED_LIBS "Build shared library" OFF "Build Shared Library")

