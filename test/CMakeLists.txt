
find_package(Threads REQUIRED)
find_package(GTest REQUIRED)


macro(_add_test_suite _name)
    foreach( arg ${ARGN} )
        list(APPEND source_list ${arg})
    endforeach()

    add_executable(${_name} ${source_list})
    add_test(NAME ${_name} COMMAND ${_name})

    target_include_directories(${_name} SYSTEM PUBLIC
                                ${GTest_INCLUDE_DIR}
                                ${GTest_Mock_INCLUDE_DIR}
                                )
    target_link_libraries(${_name} ${GTest_LIBRARIES}
                                    ${CMAKE_THREAD_LIBS_INIT}
                                    )
endmacro()


_add_test_suite(PublicTypesTests ConfigurationExceptionTest.cpp
                                StringVectorTest.cpp
                                StringBufferTest.cpp

                                $<TARGET_OBJECTS:danek-public-types>
                                )

_add_test_suite(MiscTests ToStringTest.cpp
                        UtilTest.cpp

                        $<TARGET_OBJECTS:danek-misc>
                        $<TARGET_OBJECTS:danek-config>

                        # TODO: Dependency to these should be avoided (#61)
                        $<TARGET_OBJECTS:danek-public>
                        $<TARGET_OBJECTS:danek-schematypes>
                        $<TARGET_OBJECTS:danek-security>
                        $<TARGET_OBJECTS:danek-lexparser>
                        $<TARGET_OBJECTS:DefaultSecurity>
                        )

_add_test_suite(ConfigTests ConfigItemTest.cpp
                            ConfigScopeTest.cpp
                            )

_add_test_suite(LexParserTests LexTokenTest.cpp
                            )

add_custom_target(unittest PublicTypesTests
                        COMMAND MiscTests
                        COMMAND ConfigTests
                        COMMAND LexParserTests

                        COMMENT "Running unittests\n\n"
                        VERBATIM
                        )

