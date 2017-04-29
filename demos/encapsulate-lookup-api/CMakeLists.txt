add_custom_command(OUTPUT FallbackConfiguration.h FallbackConfiguration.cpp
                        COMMAND config2cpp
                            -cfg ${CMAKE_CURRENT_SOURCE_DIR}/FallbackConfiguration.cfg
                            -class FallbackConfiguration
                            -singleton
                        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                        COMMENT "Generating FallbackConfiguration"
                        VERBATIM
                        )

add_custom_target(demo-encapsulate-lookup-api-fallback
                            DEPENDS FallbackConfiguration.cpp
                                    FallbackConfiguration.h
                            )



add_executable(demo-encapsulate-lookup-api main.cpp
                                        FooConfiguration.cpp
                                        ${CMAKE_CURRENT_BINARY_DIR}/FallbackConfiguration.cpp
                                        )
target_link_libraries(demo-encapsulate-lookup-api danek)
target_include_directories(demo-encapsulate-lookup-api PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
add_dependencies(demo-encapsulate-lookup-api demo-encapsulate-lookup-api-fallback)

