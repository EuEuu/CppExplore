#pragma once


/**
 * C++ �̳߳�ѧϰ
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

	//�̵߳������ʶ
	enum class ThreadFlag {
		kInit = 0,
		kCore = 1,//�����߳�
		kCache = 2,//Cache�߳� �ڲ�Ϊ��ִ�и���������ʱ����������
	};

	//�̵߳�״̬
	enum class ThreadState {
		kInit = 0,
		kWaiting = 1,//�ȴ�
		kRuning = 2,//������
		kStop = 3,//ֹͣ
	};

	//�̳߳����̴߳��ڵĻ�����λ��ÿ���̶߳����Զ����ID�������ʶ��״̬
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
	
	//std::function��һ��ͨ�á���̬�ĺ�����װ
	std::queue<std::function<void()>> tasks_;
};