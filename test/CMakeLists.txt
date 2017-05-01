
find_package(Threads REQUIRED)
find_package(GTest REQUIRED)


macro(_add_test _name _src _lib)
    add_executable(${_name} ${_src})
    add_test(NAME ${_name} COMMAND ${_name})

    target_include_directories(${_name} SYSTEM PUBLIC
                                ${GTest_INCLUDE_DIR}
                                ${GTest_Mock_INCLUDE_DIR}
                                )
    target_link_libraries(${_name} ${GTest_LIBRARIES}
                                    ${CMAKE_THREAD_LIBS_INIT}
                                    ${_lib}
                                    )
endmacro()

_add_test(ConfigurationExceptionTest ConfigurationExceptionTest.cpp danek)
_add_test(StringVectorTest StringVectorTest.cpp danek)


add_custom_target(unittest ConfigurationExceptionTest
                        COMMAND StringVectorTest
                        COMMENT "Running unittests\n\n"
                        VERBATIM
                        )

