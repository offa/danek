
set(TEMPLATE_DIR "${CMAKE_MODULE_PATH}/template")
set(GENERATED_DIR "${CMAKE_BINARY_DIR}/generated")

file(READ ${CMAKE_MODULE_PATH}/DefaultSecurity.cfg DEFAULT_SECURITY_CONTENT)

configure_file(${TEMPLATE_DIR}/DefaultSecurity.cpp.in
                ${GENERATED_DIR}/DefaultSecurity.cpp
                @ONLY
                )
add_library(DefaultSecurity OBJECT ${GENERATED_DIR}/DefaultSecurity.cpp)
target_include_directories(DefaultSecurity PRIVATE "${PROJECT_SOURCE_DIR}/include")




if( WIN32 )
    set(OS_TYPE_NAME "windows")
    set(OS_CFG_DIRECTORY_SEPARATOR "\\")
    set(OS_CFG_PATH_SEPARATOR ";")
else()
    set(OS_TYPE_NAME "unix")
    set(OS_CFG_DIRECTORY_SEPARATOR "/")
    set(OS_CFG_PATH_SEPARATOR ":")
endif()


configure_file(${TEMPLATE_DIR}/PlatformConfig.cpp.in
                ${GENERATED_DIR}/PlatformConfig.cpp
                @ONLY
                )
add_library(danek-platform-config ${GENERATED_DIR}/PlatformConfig.cpp)
