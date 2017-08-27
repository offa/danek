
if( CMAKE_BUILD_TYPE )
    message(STATUS "Build Type : ${CMAKE_BUILD_TYPE}")
else()
    message(STATUS "Build Type : None")
endif()


function(print_option opt name)
    message(STATUS "${name} : ${${opt}}")
endfunction()



option(UNIT_TESTS "Build Unit Tests" ON)
print_option(UNIT_TESTS "Unit Tests")

option(COVERAGE "Enable Coverage" OFF)
print_option(COVERAGE "Coverage")

option(BUILD_TOOLS "Build the tools" ON)
print_option(BUILD_TOOLS "Build Tools")

option(BUILD_SCHEMA_TESTS "Build the Schema Tests" OFF)
print_option(BUILD_SCHEMA_TESTS "Build Schema Tests")

option(BUILD_DEMOS "Build the Demos" OFF)
print_option(BUILD_DEMOS "Build Demos")

option(BUILD_SHARED_LIBS "Build Shared Library" OFF)
print_option(BUILD_SHARED_LIBS "Build shared library")

option(ENABLE_CPPCHECK "Enables CppCheck (Requires Compile Commands)" OFF)
print_option(ENABLE_CPPCHECK "Enable CppCheck")
