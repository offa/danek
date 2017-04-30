
set(TEMPLATE_DIR "${CMAKE_MODULE_PATH}/template")
set(GENERATED_DIR "${CMAKE_BINARY_DIR}/generated")

file(READ ${CMAKE_MODULE_PATH}/DefaultSecurity.cfg DEFAULT_SECURITY_CONTENT)

configure_file(${TEMPLATE_DIR}/DefaultSecurity.cpp.in
                ${GENERATED_DIR}/DefaultSecurity.cpp
                @ONLY
                )
add_library(DefaultSecurity OBJECT ${GENERATED_DIR}/DefaultSecurity.cpp)
target_include_directories(DefaultSecurity PRIVATE "${PROJECT_SOURCE_DIR}/include")
