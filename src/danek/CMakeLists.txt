subdirs(internal)

add_library(danek-public OBJECT SchemaValidator.cpp
                        Configuration.cpp
                        SchemaType.cpp
                        )
add_library(danek-public-types OBJECT StringBuffer.cpp
                                    )

add_library(danek $<TARGET_OBJECTS:danek-public>
                    $<TARGET_OBJECTS:danek-public-types>
                    $<TARGET_OBJECTS:danek-config>
                    $<TARGET_OBJECTS:danek-schematypes>
                    $<TARGET_OBJECTS:danek-lexparser>
                    $<TARGET_OBJECTS:danek-misc>
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


