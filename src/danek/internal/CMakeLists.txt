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

add_library(danek-internal SchemaLex.cpp
                        SchemaParser.cpp
                        MBChar.cpp
                        platform.cpp
                        DefaultSecurityConfiguration.cpp
                        ConfigurationImpl.cpp
                        ConfigParser.cpp
                        UidIdentifierProcessor.cpp
                        ConfigScope.cpp
                        ConfigItem.cpp
                        LexToken.cpp
                        LexBase.cpp
                        ConfigLex.cpp
                        Util.cpp
                        ToString.cpp
                        $<TARGET_OBJECTS:danek-schematypes>

                        $<TARGET_OBJECTS:DefaultSecurity>
                        )

