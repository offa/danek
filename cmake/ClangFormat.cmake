
find_program(CLANG_FORMAT_EXE clang-format DOC "Clang Format executable")

if( CLANG_FORMAT_EXE )
    set(FORMAT_SOURCE_DIRS demos include src test tests)

    add_custom_target(format 
        find ${FORMAT_SOURCE_DIRS} -name "*.h" -o -name "*.cpp" | xargs
        ${CLANG_FORMAT_EXE} -i ${FORMAT_SOURCES}
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                        COMMENT "Formatting Code"
                        VERBATIM
                        )
endif()

