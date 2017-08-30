
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

macro(add_compiler_flag)
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

