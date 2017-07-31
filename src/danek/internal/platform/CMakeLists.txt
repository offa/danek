
if( WIN32 )
    add_library(danek-platform-impl-win OBJECT Windows.cpp)
else()
    add_library(danek-platform-impl-unix OBJECT Unix.cpp)
endif()

add_library(danek-platform-impl-common OBJECT Platform.cpp)
