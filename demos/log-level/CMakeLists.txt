
add_executable(demo-log-level main.cpp
                            Logger.cpp
                            FooConfiguration.cpp
                            A.cpp 
                            B.cpp
                            )
target_link_libraries(demo-log-level danek)
