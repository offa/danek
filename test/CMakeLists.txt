
find_package(Threads REQUIRED)
find_package(GTest REQUIRED)


function(add_test_suite name)
    set(source_list)

    foreach( arg ${ARGN} )
        list(APPEND source_list ${arg})
    endforeach()

    add_executable(${name} ${source_list})
    add_test(${name} ${name})

    target_include_directories(${name} SYSTEM PUBLIC
                                ${GTest_INCLUDE_DIR}
                                ${GTest_Mock_INCLUDE_DIR}
                                )
    target_link_libraries(${name} ${GTest_LIBRARIES}
                                    ${CMAKE_THREAD_LIBS_INIT}
                                    )
endfunction()


add_test_suite(PublicMiscTests ConfigurationExceptionTest.cpp
                                StringVectorTest.cpp
                                StringBufferTest.cpp
                                $<TARGET_OBJECTS:danek-public-misc>
                                )

add_test_suite(MiscTests ToStringTest.cpp
                        UtilTest.cpp
                        UidIdentifierProcessorTest.cpp
                        UidIdentifierDummyProcessorTest.cpp
                        $<TARGET_OBJECTS:danek-misc>
                        $<TARGET_OBJECTS:danek-config-types>
                        $<TARGET_OBJECTS:danek-public-misc>
                        )

add_test_suite(ConfigTests ConfigItemTest.cpp
                            ConfigScopeTest.cpp

                            $<TARGET_OBJECTS:danek-config-types>
                            $<TARGET_OBJECTS:danek-public-misc>
                            $<TARGET_OBJECTS:danek-misc>
                            )

add_test_suite(LexParserTests LexTokenTest.cpp
                            $<TARGET_OBJECTS:danek-lexparser>
                            $<TARGET_OBJECTS:danek-config-impl>
                            $<TARGET_OBJECTS:danek-public>
                            $<TARGET_OBJECTS:danek-schematypes>
                            $<TARGET_OBJECTS:danek-security>
                            $<TARGET_OBJECTS:DefaultSecurity>
                            $<TARGET_OBJECTS:danek-public-misc>
                            $<TARGET_OBJECTS:danek-config-types>
                            $<TARGET_OBJECTS:danek-misc>
                            $<TARGET_OBJECTS:danek-platform-config>
                            $<TARGET_OBJECTS:danek-platform-impl>
                            )

add_test_suite(SchemaTests SchemaRuleInfoTest.cpp
                            )

add_test_suite(PlatformTests PlatformTest.cpp
                            $<TARGET_OBJECTS:danek-platform-config>
                            )

add_test_suite(SchemaValidatorTest SchemaValidatorTest.cpp
                            $<TARGET_OBJECTS:danek-public>
                            $<TARGET_OBJECTS:danek-public-misc>
                            $<TARGET_OBJECTS:danek-schematypes>
                            $<TARGET_OBJECTS:danek-config-types>
                            $<TARGET_OBJECTS:danek-lexparser>
                            $<TARGET_OBJECTS:danek-platform-impl>
                            $<TARGET_OBJECTS:danek-platform-config>
                            $<TARGET_OBJECTS:danek-misc>
                            $<TARGET_OBJECTS:danek-security>
                            $<TARGET_OBJECTS:DefaultSecurity>
                            $<TARGET_OBJECTS:danek-config-impl>
                            )




add_custom_target(unittest PublicMiscTests
                        COMMAND MiscTests
                        COMMAND ConfigTests
                        COMMAND LexParserTests
                        COMMAND SchemaTests
                        COMMAND PlatformTests
                        COMMAND SchemaValidatorTest

                        COMMENT "Running unittests\n\n"
                        VERBATIM
                        )

