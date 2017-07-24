subdirs(internal)

add_library(danek-public OBJECT SchemaValidator.cpp
                        Configuration.cpp
                        SchemaType.cpp
                        )
add_library(danek-public-misc OBJECT StringBuffer.cpp
                                    PatternMatch.cpp
                                    )

add_library(danek $<TARGET_OBJECTS:danek-public>
                    $<TARGET_OBJECTS:danek-public-misc>
                    $<TARGET_OBJECTS:danek-config-types>
                    $<TARGET_OBJECTS:danek-config-impl>
                    $<TARGET_OBJECTS:danek-schematypes>
                    $<TARGET_OBJECTS:danek-lexparser>
                    $<TARGET_OBJECTS:danek-misc>
                    $<TARGET_OBJECTS:danek-platform>
                    $<TARGET_OBJECTS:danek-platform-impl>
                    $<TARGET_OBJECTS:danek-security>
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


