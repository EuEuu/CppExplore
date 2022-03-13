#include "thread_pool.hpp"

ThreadPool::ThreadPool(ThreadPoolConfig config) :is_valid_config_(false), config_(config)
{
	is_valid_config_ = IsValidConfig(config);

	thread_id_ = 0;

	std::cout << __FUNCTION__ << std::endl;
}

ThreadPool::sptr ThreadPool::make(ThreadPoolConfig config)
{
	return std::make_shared<ThreadPool>(config);
}

ThreadPool::~ThreadPool()
{
	std::cout << __FUNCTION__ << std::endl;
}

bool ThreadPool::IsValidConfig(ThreadPoolConfig config)
{
	return (config.core_threads > 0 && config.max_threads >= config.core_threads && config.timeout.count() > 0);
}


bool ThreadPool::IsValidConfig() const
{
	return is_valid_config_;
}

bool ThreadPool::Start()
{
	if (!is_valid_config_) return false;

	int core_thread_num = config_.core_threads;
	while (core_thread_num-- > 0)
	{
		AddThread(GetNextThreadId(), ThreadFlag::kCore);
	}
	std::cout << "Init thread finish" << std::endl;
	return true;
}

void ThreadPool::AddThread(int id, ThreadFlag thread_flag)
{
	std::cout << "Add thread " << id << ", flag " << static_cast<int>(thread_flag) << std::endl;
	std::shared_ptr<ThreadWrapper> thread_ptr = std::make_shared<ThreadWrapper>();
	thread_ptr->id = id;
	thread_ptr->flag = thread_flag;

	auto func = [this, &thread_ptr] {

	};

	thread_ptr->sptr = std::make_shared<std::thread>(std::move(func));
	if (thread_ptr->sptr->joinable()) {
		thread_ptr->sptr->detach();
	}

	worker_threads_.emplace_back(std::move(thread_ptr));
}

int ThreadPool::GetNextThreadId()
{
	thread_id_++;
}
