include(CheckCXXCompilerFlag)

function(add_compile_options_if_supported)
    foreach(flag ${ARGN})
        set(option_var option_supported_${flag})
        check_cxx_compiler_flag(${flag} ${option_var})

        if( ${option_var} )
            add_compile_options(${flag})
        endif()
    endforeach()
endfunction()

macro(add_c_flag)
    foreach(flag ${ARGN})
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}")
    endforeach()
endmacro()

macro(add_cxx_flag)
    foreach(flag ${ARGN})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
    endforeach()
endmacro()

macro(add_common_flags)
    foreach(flag ${ARGN})
        add_c_flag(${flag})
        add_cxx_flag(${flag})
    endforeach()
endmacro()

macro(add_linker_flag)
    foreach(flag ${ARGN})
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${flag}")
    endforeach()
endmacro()

