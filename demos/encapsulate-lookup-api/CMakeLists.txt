add_custom_target(demo-encapsulate-lookup-api-fallback
                        COMMAND config2cpp
                            -cfg FallbackConfiguration.cfg 
                            -class FallbackConfiguration
                            -singleton
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        COMMENT "Generating FallbackConfiguration"
                        VERBATIM
                        )

add_executable(demo-encapsulate-lookup-api main.cpp
                                        FooConfiguration.cpp
                                        FallbackConfiguration.cpp
                                        )
target_link_libraries(demo-encapsulate-lookup-api danek)
add_dependencies(demo-encapsulate-lookup-api demo-encapsulate-lookup-api-fallback)

