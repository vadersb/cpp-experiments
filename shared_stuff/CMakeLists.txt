add_library(shared_stuff STATIC
        test/test_func.h
        test/test_func.cpp
        memory/memory_pool.h
        memory/internal/memory_pool_bucket.cpp
        memory/internal/memory_pool_bucket.h
        memory/internal/memory_pool_settings.h
        memory/memory_poolable.h
        memory/memory_reference_counted.h
        memory/memory_reference_counted.cpp
        memory/memory_rcptr.h
        memory/memory_allocator.h
        memory/memory_wptr.h
        utils/utils_cast.h
        utils/delegate.h memory/internal/memory_pool_settings.cpp)

target_link_libraries(shared_stuff spdlog)
target_include_directories(shared_stuff PUBLIC test memory utils)
