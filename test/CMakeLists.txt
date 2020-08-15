
find_package(GTest REQUIRED)


function(add_test_suite name)
    add_test(${name} ${name})
    target_link_libraries(${name} PRIVATE
        GTest::GTest
        GTest::Mock
        GTest::Main
        )
endfunction()


add_executable(PublicMiscTests ConfigurationExceptionTest.cpp
                            StringVectorTest.cpp
                            StringBufferTest.cpp
                            )
target_link_libraries(PublicMiscTests PRIVATE
                                    danek-public-misc
                                    )
add_test_suite(PublicMiscTests)



add_executable(MiscTests ToStringTest.cpp
                        UtilTest.cpp
                        UidIdentifierProcessorTest.cpp
                        UidIdentifierDummyProcessorTest.cpp
                        )
target_link_libraries(MiscTests PRIVATE
                                danek-misc
                                danek-config-types
                                danek-public-misc
                                )
add_test_suite(MiscTests)



add_executable(ConfigTests ConfigItemTest.cpp
                        ConfigScopeTest.cpp
                        )
target_link_libraries(ConfigTests PRIVATE
                                danek-config-types
                                danek-public-misc
                                danek-misc
                                )
add_test_suite(ConfigTests)



add_executable(LexParserTests LexTokenTest.cpp
                            )
target_link_libraries(LexParserTests PRIVATE
                                    danek-lexparser
                                    danek-config-impl
                                    danek-public
                                    danek-schematypes
                                    danek-security
                                    danek-public-misc
                                    danek-config-types
                                    danek-misc
                                    danek-platform-config
                                    danek-platform-impl
                                    )
add_test_suite(LexParserTests)



add_executable(SchemaTests SchemaRuleInfoTest.cpp
                            )
add_test_suite(SchemaTests)



add_executable(PlatformTests PlatformTest.cpp
                            )
target_link_libraries(PlatformTests PRIVATE
                                    danek-platform-config
                                    )
add_test_suite(PlatformTests)



add_executable(SchemaValidatorTests SchemaValidatorTest.cpp
                                    )
target_link_libraries(SchemaValidatorTests PRIVATE
                                        danek-public
                                        danek-config-impl
                                        danek-public-misc
                                        danek-schematypes
                                        danek-config-types
                                        danek-lexparser
                                        danek-platform-impl
                                        danek-platform-config
                                        danek-misc
                                        danek-security
                                        )
add_test_suite(SchemaValidatorTests)




add_custom_target(unittest PublicMiscTests
                        COMMAND MiscTests
                        COMMAND ConfigTests
                        COMMAND LexParserTests
                        COMMAND SchemaTests
                        COMMAND PlatformTests
                        COMMAND SchemaValidatorTests

                        COMMENT "Running unittests\n\n"
                        VERBATIM
                        )
