
find_program(CPPCHECK "cppcheck" NO_CMAKE_FIND_ROOT_PATH DOC "CppCheck Executable")

if( CPPCHECK )
    if( NOT CMAKE_EXPORT_COMPILE_COMMANDS )
        message(STATUS "'CMAKE_EXPORT_COMPILE_COMMANDS' not enabled but required by CppCheck")
    endif()

    add_custom_target(cppcheck)
    add_custom_command(TARGET cppcheck POST_BUILD
                        COMMAND ${CPPCHECK} --error-exitcode=1
                                            --enable=all
                                            --project=${CMAKE_BINARY_DIR}/compile_commands.json
                                            --report-progress
                                            --suppress='*:*test/*'
                                            --suppress=missingInclude
                                            --suppress=unmatchedSuppression
                                            --suppressions-list=${CMAKE_MODULE_PATH}/CppCheck.suppressions
                                            "${PROJECT_SOURCE_DIR}/src"
                                            "${PROJECT_SOURCE_DIR}/include"
                        COMMENT "Running CppCheck ..."
                        )
else()
    message(STATUS "CppCheck not found")
endif()

