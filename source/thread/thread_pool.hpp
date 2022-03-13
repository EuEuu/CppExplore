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
		std::atomic_int id{ 0 };
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

	void AddThread(int id, ThreadFlag);
private:

	int GetNextThreadId();

private:
	ThreadPoolConfig config_;
	bool is_valid_config_;
	std::atomic_int thread_id_;

	std::list<std::shared_ptr<ThreadWrapper>> worker_threads_;
	
	//std::function是一种通用、多态的函数封装
	std::queue<std::function<void()>> tasks_;
};