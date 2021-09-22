#hello world
add_executable(hello_world hello_world/main.cpp)
target_link_libraries(hello_world shared_stuff)

#raylib hello world
add_executable(raylib_hello_world raylib_hello_world/main.cpp)
target_link_libraries(raylib_hello_world PRIVATE shared_stuff raylib)

#memory_pool
add_executable(memory_pool memory_pool/main.cpp)
target_link_libraries(memory_pool shared_stuff)

#delegate
add_executable(delegate delegate/main.cpp delegate/delegate_types.h delegate/delegate_types.cpp)
target_link_libraries(delegate shared_stuff)

#threading tasks
add_executable(threading_tasks threading_tasks/main.cpp)
target_link_libraries(threading_tasks shared_stuff)