#include "thread_pool.hpp"

ThreadPool::ThreadPool(ThreadPoolConfig config) :is_valid_config_(false), config_(config)
{
	is_valid_config_ = IsValidConfig(config);
	is_available_.store(is_valid_config_);

	thread_id_ = 0;
	waiting_thread_num_ = 0;
	total_function_num_ = 0;

	is_shudown_.store(false);
	is_shutdown_now_.store(false);

	std::cout << __FUNCTION__ << std::endl;
}

ThreadPool::sptr ThreadPool::make(ThreadPoolConfig config)
{
	return std::make_shared<ThreadPool>(config);
}

ThreadPool::~ThreadPool()
{
	std::unique_lock<std::mutex> lock(task_mtx_);
	Shutdown(true);
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
	thread_ptr->state = ThreadState::kInit;

	auto func = [this, thread_ptr] {
		for (;;) {

			std::function<void()> task;
			{
				//std::unique_lock<std::mutex> lock(task_mtx_);
				
				if (thread_ptr->state == ThreadState::kStop) {
					break;//该线程退出
				}
				std::cout << "thread: " << thread_ptr->id << " waiting" << std::endl;
				thread_ptr->state = ThreadState::kWaiting;//等待任务
				waiting_thread_num_++;//记录等到的线程数

				bool is_timeout = false;

				std::unique_lock<std::mutex> lock(task_mtx_);

				if (thread_ptr->flag == ThreadFlag::kCore) {//线程池的核心线程
					//阻塞等待，直到满足条件
					cv_.wait(lock, [this, thread_ptr] {
						return ((thread_ptr->state == ThreadState::kStop) || !tasks_.empty() || is_shudown_ || is_shutdown_now_);
						}
					);
				}
				else {
					//阻塞等待，直到满足条件
					cv_.wait_for(lock, config_.timeout, [this, thread_ptr] {
						return ((thread_ptr->state == ThreadState::kStop) || !tasks_.empty() || is_shudown_ || is_shutdown_now_);
						}
					);

					is_timeout = !((thread_ptr->state == ThreadState::kStop) || !tasks_.empty() || is_shudown_ || is_shutdown_now_);
				};

				waiting_thread_num_--;//记录等到的线程数

				std::cout << "thread: " << thread_ptr->id << " waiting end" << std::endl;

				if (is_timeout) {
					thread_ptr->state = ThreadState::kStop;//超时就退出
					std::cout << "thread: " << thread_ptr->id << " timeout, exit..." << std::endl;
					break;
				}
				if (is_shutdown_now_) {
					std::cout << "thread: " << thread_ptr->id << " shutdown now" << std::endl;
					break;//立即退出
				}
				if (is_shudown_ && tasks_.empty()) {
					std::cout << "thread: " << thread_ptr->id << " shutdown" << std::endl;
					break;//退出
				}

				thread_ptr->state = ThreadState::kRuning;
				std::cout << "thread: " << thread_ptr->id << " runing" << std::endl;
				task = std::move(tasks_.front());
				tasks_.pop();

			}

			task();

			std::cout << "thread: " << thread_ptr->id << " runing end" << std::endl;
		}

		std::cout << "thread: " << thread_ptr->id << " exit" << std::endl;
	};

	thread_ptr->sptr = std::make_shared<std::thread>(std::move(func));
	if (thread_ptr->sptr->joinable()) {
		thread_ptr->sptr->detach();
	}

	worker_threads_.emplace_back(std::move(thread_ptr));
}

void ThreadPool::Shutdown(bool is_now)
{
	if (!is_available_)return;
	if (is_now) {
		is_shutdown_now_.store(true);
	}
	else {
		is_shudown_.store(true);
	}
	
	cv_.notify_all();//通知所有线程
	is_available_.store(false);
}

void ThreadPool::Resize(int thread_num)
{
	if (thread_num < config_.core_threads)return;
	int old_threads_num = worker_threads_.size();
	std::cout << "old threads num: " << old_threads_num << ", resize: " << thread_num << std::endl;

	if (thread_num > old_threads_num) {
		while (thread_num-- > old_threads_num)
		{
			AddThread(GetNextThreadId(), ThreadFlag::kCore);
		}
	}
	else {
		int diff = old_threads_num - thread_num;
		auto iter = worker_threads_.begin();
		while (iter != worker_threads_.end())
		{
			if (diff <= 0)break;

			auto thread_ptr = *iter;
			if (thread_ptr->flag == ThreadFlag::kCache && thread_ptr->state == ThreadState::kWaiting) {
				thread_ptr->state == ThreadState::kStop;
				--diff;
				iter = worker_threads_.erase(iter);
			}
			else {
				++iter;
			}

		}
		cv_.notify_all();
	}
}

int ThreadPool::GetNextThreadId()
{
	return thread_id_++;
}
