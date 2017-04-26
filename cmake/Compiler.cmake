
macro(add_c_flag)
    foreach(_flag ${ARGN})
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${_flag}")
    endforeach()
endmacro()

macro(add_cxx_flag)
    foreach(_flag ${ARGN})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_flag}")
    endforeach()
endmacro()

macro(add_compiler_flag)
    foreach(_flag ${ARGN})
        add_c_flag(${_flag})
        add_cxx_flag(${_flag})
    endforeach()
endmacro()

macro(add_linker_flag)
    foreach(_flag ${ARGN})
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${_flag}")
    endforeach()
endmacro()

