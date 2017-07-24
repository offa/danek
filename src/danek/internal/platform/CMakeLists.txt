
if( WIN32 )
    add_library(danek-platform-impl-win Windows.cpp)
else()
    add_library(danek-platform-impl-unix Unix.cpp)
endif()
