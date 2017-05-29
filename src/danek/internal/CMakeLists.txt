add_library(danek-schematypes SchemaTypeBoolean.cpp
                            SchemaTypeDurationMicroseconds.cpp
                            SchemaTypeDurationMilliseconds.cpp
                            SchemaTypeDurationSeconds.cpp
                            SchemaTypeEnum.cpp
                            SchemaTypeFloat.cpp
                            SchemaTypeFloatWithUnits.cpp
                            SchemaTypeInt.cpp
                            SchemaTypeIntWithUnits.cpp
                            SchemaTypeMemorySizeBytes.cpp
                            SchemaTypeMemorySizeKB.cpp
                            SchemaTypeMemorySizeMB.cpp
                            SchemaTypeScope.cpp
                            SchemaTypeString.cpp
                            SchemaTypeUnitsWithFloat.cpp
                            SchemaTypeUnitsWithInt.cpp
                            SchemaTypeList.cpp
                            SchemaTypeTable.cpp
                            SchemaTypeTuple.cpp
                            SchemaTypeTypedef.cpp
                            )

add_library(danek-misc platform.cpp
                        UidIdentifierProcessor.cpp
                        Util.cpp
                        ToString.cpp
                        MBChar.cpp
                        )

add_library(danek-lexparser SchemaLex.cpp
                        SchemaParser.cpp
                        ConfigParser.cpp
                        LexToken.cpp
                        LexBase.cpp
                        ConfigLex.cpp
                        )

add_library(danek-internal DefaultSecurityConfiguration.cpp
                        ConfigurationImpl.cpp
                        ConfigScope.cpp
                        ConfigItem.cpp
                        $<TARGET_OBJECTS:DefaultSecurity>
                        )
target_link_libraries(danek-internal danek-schematypes
                                    danek-lexparser
                                    danek-misc
                                    )

