
macro(_print_option _option _name)
    message(STATUS "${_name} : ${${_option}}")
endmacro()


option(UNIT_TESTS "Build Unit Tests" ON)
option(COVERAGE "Enable Coverage" OFF)
option(BUILD_TOOLS "Build the tools" ON)
option(BUILD_SCHEMA_TESTS "Build the Schema Tests" OFF)
option(BUILD_DEMOS "Build the Demos" OFF)
option(BUILD_SHARED_LIBS "Build Shared Library" OFF)


_print_option(UNIT_TESTS "Unit Tests")
_print_option(COVERAGE "Coverage")
_print_option(BUILD_TOOLS "Build Tools")
_print_option(BUILD_SCHEMA_TESTS "Build Schema Tests")
_print_option(BUILD_DEMOS "Build Demos")
_print_option(BUILD_SHARED_LIBS "Build shared library")

