add_subdirectory(internal)

add_library(danek-public SchemaValidator.cpp
                        Configuration.cpp
                        SchemaType.cpp
                        )
add_library(danek-public-misc StringBuffer.cpp
                                PatternMatch.cpp
                                )


add_library(danek $<TARGET_OBJECTS:DefaultSecurity>)
target_link_libraries(danek PUBLIC
                        danek-public
                        danek-config-impl
                        danek-public-misc
                        )
target_link_libraries(danek PRIVATE
                        danek-config-types
                        danek-schematypes
                        danek-lexparser
                        danek-misc
                        danek-platform-config
                        danek-platform-impl
                        danek-security
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


