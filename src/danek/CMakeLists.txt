add_library(danek SchemaLex.cpp
                SchemaParser.cpp
                MBChar.cpp
                SchemaValidator.cpp
                platform.cpp
                Configuration.cpp
                DefaultSecurityConfiguration.cpp
                ConfigurationImpl.cpp
                ConfigParser.cpp
                UidIdentifierProcessor.cpp
                ConfigScope.cpp
                ConfigScopeEntry.cpp
                ConfigItem.cpp
                LexToken.cpp
                LexBase.cpp
                ConfigLex.cpp
                StringBuffer.cpp
                SchemaType.cpp
                SchemaTypeBoolean.cpp
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
                Util.cpp

                $<TARGET_OBJECTS:DefaultSecurity>
                )

install(TARGETS danek
                LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
                ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
                RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
                )
install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/danek/"
                DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
                PATTERN "danek/internal/*" EXCLUDE
                )


