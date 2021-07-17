add_library(shared_stuff STATIC test/test_func.h test/test_func.cpp)
target_include_directories(shared_stuff PUBLIC test)