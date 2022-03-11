#pragma once


/**
 * C++ �̳߳�ѧϰ
 */

#include <iostream>
#include <chrono>
#include <queue>
#include <thread>

class ThreadPool
{
public:
	/**
	 * core_threads�������߳������̳߳�������ӵ�е��߳�������ʼ���ͻὨ�ã���פ���̳߳�
	 * max_threads���̳߳�����ཨ�����߳�����max_threads >= core_threads,����������̫�࣬
	 *				core�߳�ִ�в�����ʱ���ڲ��Ὠ��������߳���ִ�������ڲ��߳������ᳬ��max_threads
	 * max_tasks���������������δʹ��
	 * timeout��Cache�̳߳�ʱʱ�䣬Cache�߳���ָmax_threads-core_threads���̣߳���timeoutʱ����û������ִ��ʱ�����߳̾ͻᱻ����
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