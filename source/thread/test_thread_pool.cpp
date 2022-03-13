#include <iostream>

#include "thread_pool.hpp"


int main(int argc, char** argv) {


	ThreadPool::ThreadPoolConfig config;
	config.core_threads = 3;
	config.max_threads = 5;
	config.timeout = std::chrono::seconds(1);
	auto pool = ThreadPool::make(config);

	std::cout << pool->IsValidConfig() << std::endl;;

	return 0;
}