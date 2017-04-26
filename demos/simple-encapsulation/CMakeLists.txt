add_custom_target(demo-simple-encapsulation-fallback
                        COMMAND config2cpp
                            -cfg FallbackConfiguration.cfg 
                            -class FallbackConfiguration
                            -singleton
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        COMMENT "Generating FallbackConfiguration"
                        VERBATIM
                        )


add_executable(demo-simple-encapsulation main.cpp
                                        FooConfiguration.cpp 
                                        FallbackConfiguration.cpp
                                        )
target_link_libraries(demo-simple-encapsulation danek)
add_dependencies(demo-simple-encapsulation demo-simple-encapsulation-fallback)


