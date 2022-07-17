#hello world
add_executable(hello_world hello_world/main.cpp)
target_link_libraries(hello_world shared_stuff)



#raylib hello world
if (DEFINED RAYLIB_DIR)

    add_executable(raylib_hello_world raylib_hello_world/main.cpp)
    target_link_libraries(raylib_hello_world PRIVATE shared_stuff raylib)

endif()



#memory_pool
add_executable(memory_pool memory_pool/main.cpp)
target_link_libraries(memory_pool shared_stuff)


#memory pool benchmark
add_executable(memory_pool_benchmark memory_pool_benchmark/main.cpp memory_pool_benchmark/main.h)
target_link_libraries(memory_pool_benchmark PRIVATE shared_stuff)

if (DEFINED MIMALLOC_DIR)
    target_link_libraries(memory_pool_benchmark PRIVATE mimalloc-static)
endif()


#delegate
add_executable(delegate delegate/main.cpp delegate/delegate_types.h delegate/delegate_types.cpp)
target_link_libraries(delegate shared_stuff)


#------------------------------------------------
#threading tasks
add_executable(threading_tasks threading_tasks/main.cpp threading_tasks/main.h )
target_link_libraries(threading_tasks PRIVATE shared_stuff )
if (DEFINED ENKITS_DIR)
    target_link_libraries(threading_tasks PRIVATE enkiTS )
    target_sources(threading_tasks PRIVATE threading_tasks/tasks_enkits.cpp )
endif()


#threading std
add_executable(threading_std threading_std/main.cpp threading_std/main.h threading_std/async_stess_test.cpp)
target_link_libraries(threading_std shared_stuff)


#sokol hello world
if (DEFINED SOKOL_DIR)

    add_executable(sokol_hello_world sokol_hello_world/main.cpp sokol_hello_world/main.h)
    target_link_libraries(sokol_hello_world PRIVATE shared_stuff sokol)

endif()