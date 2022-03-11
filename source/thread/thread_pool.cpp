#include "thread_pool.hpp"

ThreadPool::ThreadPool(ThreadPoolConfig config) :is_valid_config_(false), config_(config)
{
	is_valid_config_ = IsValidConfig(config);
	
	std::cout << __FUNCTION__ << std::endl;
}

ThreadPool::~ThreadPool()
{
	std::cout << __FUNCTION__ << std::endl;
}

bool ThreadPool::IsValidConfig(ThreadPoolConfig config)
{
	return (config.core_threads > 0 && config.max_threads >= config.core_threads && config.timeout.count() > 0);
}

bool ThreadPool::IsValidConfig()
{
	return is_valid_config_;
}
