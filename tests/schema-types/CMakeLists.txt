
set(CFG_FILE "schema-type-tests.cfg")


add_executable(test-schema-types main.cpp)
target_link_libraries(test-schema-types danek)

add_custom_target(run-schema-test
                    test-schema-types
                    -cfg
                    ${CMAKE_CURRENT_SOURCE_DIR}/${CFG_FILE}
                    DEPENDS test-schema-types
                    VERBATIM
                    )

add_custom_target(valgrind-schema-test
                    valgrind
                    ${CMAKE_CURRENT_BINARY_DIR}/test-schema-types
                    -cfg
                    ${CMAKE_CURRENT_SOURCE_DIR}/${CFG_FILE}
                    )

