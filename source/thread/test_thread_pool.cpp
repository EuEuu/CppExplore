#include <iostream>
#include <thread>

#include "thread_pool.hpp"


int main(int argc, char** argv) {


	ThreadPool::ThreadPoolConfig config;
	config.core_threads = 3;
	config.max_threads = 5;
	config.timeout = std::chrono::seconds(1);
	auto pool = ThreadPool::make(config);

	pool->Start();

	std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "thread size " << pool->GetTotalThreadSize() << std::endl;
    std::atomic<int> index;
    index.store(0);
    std::thread t([&]() {
        for (int i = 0; i < 10; ++i) {
            pool->Run([&]() {
                std::cout << "function " << index.load() << std::endl;
                //std::this_thread::sleep_for(std::chrono::seconds(4));
                index++;
                });
            // std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        });
    t.detach();
    std::cout << "=================" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "thread size " << pool->GetTotalThreadSize() << std::endl;
    std::cout << "waiting size " << pool->GetWaitingThreadSize() << std::endl;
    std::cout << "---------------" << std::endl;

	return 0;
}