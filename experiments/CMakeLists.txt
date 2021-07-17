#hello world
add_executable(hello_world hello_world/main.cpp)
target_link_libraries(hello_world shared_stuff)

#raylib hello world
add_executable(raylib_hello_world raylib_hello_world/main.cpp)
target_link_libraries(raylib_hello_world PRIVATE shared_stuff raylib)