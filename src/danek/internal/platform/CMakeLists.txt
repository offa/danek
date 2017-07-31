
if( WIN32 )
    set(PLATFORM_IMPL Windows.cpp)
else()
    set(PLATFORM_IMPL Unix.cpp)
endif()

add_library(danek-platform-impl-common OBJECT Platform.cpp ${PLATFORM_IMPL})
