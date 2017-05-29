add_library(danek-schematypes OBJECT SchemaTypeBoolean.cpp
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

add_library(danek-misc OBJECT platform.cpp
                        UidIdentifierProcessor.cpp
                        Util.cpp
                        ToString.cpp
                        MBChar.cpp
                        )

add_library(danek-lexparser OBJECT SchemaLex.cpp
                        SchemaParser.cpp
                        ConfigParser.cpp
                        LexToken.cpp
                        LexBase.cpp
                        ConfigLex.cpp
                        )

add_library(danek-config-impl OBJECT ConfigurationImpl.cpp
                                    )

add_library(danek-config-types OBJECT ConfigScope.cpp
                                    ConfigItem.cpp
                                    )

add_library(danek-security OBJECT DefaultSecurityConfiguration.cpp
                        )

