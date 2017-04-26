
add_executable(demo-extended-schema-validator main.cpp
                                            FooConfiguration.cpp
                                            SchemaTypeHex.cpp
                                            )
target_link_libraries(demo-extended-schema-validator danek)
