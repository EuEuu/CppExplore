#pragma once


/**
 * C++ 线程池学习
 */

#include <iostream>
#include <chrono>
#include <queue>
#include <thread>

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

	explicit ThreadPool(ThreadPoolConfig config);

	~ThreadPool();

	static bool IsValidConfig(ThreadPoolConfig config);

private:
	 
	bool IsValidConfig();

private:
	ThreadPoolConfig config_;
	bool is_valid_config_;
};