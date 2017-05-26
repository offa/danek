subdirs(internal)

add_library(danek SchemaValidator.cpp
                Configuration.cpp
                StringBuffer.cpp
                SchemaType.cpp
                )
target_link_libraries(danek danek-internal)


install(TARGETS danek
                LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
                ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
                RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
                )
install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/danek/"
                DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
                PATTERN "danek/internal/*" EXCLUDE
                )


