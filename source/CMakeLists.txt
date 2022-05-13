

add_executable(construction_test "construction/construction_test.cpp")

add_executable(test_erase "stl_vector/test_erase.cpp")

add_executable(test_thread_pool 
			thread/test_thread_pool.cpp
			thread/thread_pool.cpp)

add_executable(test_function std_function/test_function.cpp)

add_executable(test_rvalue rvalue/test_rvalue.cpp)

add_executable(test_kmp KMP/KMP.cpp)