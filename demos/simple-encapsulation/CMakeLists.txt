add_custom_command(OUTPUT FallbackConfiguration.h FallbackConfiguration.cpp
                        COMMAND config2cpp
                            -cfg ${CMAKE_CURRENT_SOURCE_DIR}/FallbackConfiguration.cfg
                            -class FallbackConfiguration
                            -singleton
                        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                        COMMENT "Generating FallbackConfiguration"
                        VERBATIM
                        )


add_custom_target(demo-simple-encapsulation-fallback
                            DEPENDS FallbackConfiguration.cpp
                                    FallbackConfiguration.h
                                    )

add_executable(demo-simple-encapsulation main.cpp
                                        FooConfiguration.cpp
                                        ${CMAKE_CURRENT_BINARY_DIR}/FallbackConfiguration.cpp
                                        )
target_include_directories(demo-simple-encapsulation
                                PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
                                PRIVATE ${DEMO_COMMON_SRC_DIR}
                                )
target_link_libraries(demo-simple-encapsulation danek)
add_dependencies(demo-simple-encapsulation demo-simple-encapsulation-fallback)

