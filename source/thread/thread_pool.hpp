#pragma once


/**
 * C++ 线程池学习
 */

#include <iostream>
#include <chrono>
#include <queue>
#include <thread>
#include <memory>
#include <list>
#include <atomic>
#include <future>
#include <mutex>
#include <functional>

class ThreadPool
{
public:
	/**
	 * core_threads：核心线程数，线程池中至少拥有的线程数，初始化就会建好，常驻于线程池
	 * max_threads：线程池中最多建立的线程数。max_threads >= core_threads,当任务数量太多，
	 *				core线程执行不过来时，内部会建立更多的线程来执行任务，内部线程数不会超过max_threads
	 * max_tasks：最大任务数量，未使用
	 * timeout：Cache线程超时时间，Cache线程是指max_threads-core_threads的线程，当timeout时间内没有任务执行时，此线程就会被回收
	 */
	struct ThreadPoolConfig
	{
		int core_threads;
		int max_threads;
		int max_tasks;
		std::chrono::seconds timeout;
	};

	//线程的种类标识
	enum class ThreadFlag {
		kInit = 0,
		kCore = 1,//核心线程
		kCache = 2,//Cache线程 内部为了执行更多任务零时创建出来的
	};

	//线程的状态
	enum class ThreadState {
		kInit = 0,
		kWaiting = 1,//等待
		kRuning = 2,//运行中
		kStop = 3,//停止
	};

	//线程池中线程存在的基本单位，每个线程都有自定义的ID、种类标识和状态
	struct ThreadWrapper {
		std::shared_ptr<std::thread> sptr{ nullptr };
		std::atomic<int> id{ 0 };
		ThreadFlag flag;
		ThreadState state;
	};

	using sptr = std::shared_ptr<ThreadPool>;

	explicit ThreadPool(ThreadPoolConfig config);

	static sptr make(ThreadPoolConfig config);

	~ThreadPool();

	static bool IsValidConfig(ThreadPoolConfig config);

	bool IsValidConfig() const;

	bool Start();

	void Shutdown(bool is_now);

	//放任务进线程池
	//...Args 为可变模板参数
	//decltype(表达式)
	//std::future提供了异步访问操作结果的机制
	//用到了尾返回类型推导
	template<typename F,typename ...Args>
	auto Run(F&& f, Args&& ...args) -> std::shared_ptr<std::future<decltype(f(args...))>> {
		if (is_shudown_ || is_shutdown_now_ || !IsAvailable())return nullptr;
		if (GetWaitingThreadSize() == 0 && GetTotalThreadSize() < config_.core_threads) {
			AddThread(GetNextThreadId(), ThreadFlag::kCache);
		}

		//连接函数和参数定义，特殊函数类型，避免左右值错误
		//std::function 函数包装器 可将普通函数、lambda函数、std::bind和成员函数进行包装
		//std::forward 称为完美转发，将会完整保留参数的引用类型进行转发
		std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

		auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

		std::function<void()> warpper_func = [task_ptr] {
			(*task_ptr)();
		};

		total_function_num_++;

		{
			std::unique_lock<std::mutex> lock(task_mtx_);
			tasks_.emplace(warpper_func);
			cv_.notify_one();
		}

		return std::make_shared<std::future<decltype(f(args...))>>(std::move(task_ptr->get_future()));
	}

	// 获取正在处于等待状态的线程的个数
	int GetWaitingThreadSize() const { return this->waiting_thread_num_.load(); }

	// 获取线程池中当前线程的总个数
	int GetTotalThreadSize() const { return this->worker_threads_.size(); }

	// 获取当前线程池已经执行过的函数个数
	int GetRunnedFuncNum() const { return total_function_num_.load(); }

	// 当前线程池是否可用
	bool IsAvailable() { return is_available_.load(); }
private:

	int GetNextThreadId();

	void AddThread(int id, ThreadFlag);

	void Resize(int thread_num);

private:
	ThreadPoolConfig config_;
	bool is_valid_config_;
	std::atomic_int thread_id_;

	std::list<std::shared_ptr<ThreadWrapper>> worker_threads_;

	//std::function是一种通用、多态的函数封装
	std::queue<std::function<void()>> tasks_;//任务队列
	std::mutex task_mtx_;//锁住任务队列
	std::condition_variable cv_;//信号量

	std::atomic<int> waiting_thread_num_;
	std::atomic<int> total_function_num_;

	std::atomic_bool is_shudown_;
	std::atomic_bool is_shutdown_now_;
	std::atomic<bool> is_available_;
};