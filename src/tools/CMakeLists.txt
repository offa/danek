
add_executable(config2cpp config2cpp-main.cpp Config2Cpp.cpp)
target_link_libraries(config2cpp PRIVATE danek)

add_executable(config2cpp-nocheck config2cpp-nocheck-main.cpp Config2Cpp.cpp)
target_link_libraries(config2cpp-nocheck PRIVATE danek)

add_executable(config4cpp-bin config4cpp.cpp)
target_link_libraries(config4cpp-bin PRIVATE danek)

