
add_executable(demo-extended-schema-validator main.cpp
                                            FooConfiguration.cpp
                                            SchemaTypeHex.cpp
                                            )
target_link_libraries(demo-extended-schema-validator danek)
target_include_directories(demo-extended-schema-validator
                                PRIVATE ${DEMO_COMMON_SRC_DIR}
                                )

