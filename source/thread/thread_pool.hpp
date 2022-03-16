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
#include <atomic>
#include <future>
#include <mutex>
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

	//��������̳߳�
	//...Args Ϊ�ɱ�ģ�����
	//decltype(���ʽ)
	//std::future�ṩ���첽���ʲ�������Ļ���
	//�õ���β���������Ƶ�
	template<typename F,typename ...Args>
	auto Run(F&& f, Args&& ...args) -> std::shared_ptr<std::future<decltype(f(args...))>> {
		if (is_shudown_ || is_shutdown_now_ || !IsAvailable())return nullptr;
		if (GetWaitingThreadSize() == 0 && GetTotalThreadSize() < config_.core_threads) {
			AddThread(GetNextThreadId(), ThreadFlag::kCache);
		}

		//���Ӻ����Ͳ������壬���⺯�����ͣ���������ֵ����
		//std::function ������װ�� �ɽ���ͨ������lambda������std::bind�ͳ�Ա�������а�װ
		//std::forward ��Ϊ����ת�����������������������������ͽ���ת��
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

	// ��ȡ���ڴ��ڵȴ�״̬���̵߳ĸ���
	int GetWaitingThreadSize() const { return this->waiting_thread_num_.load(); }

	// ��ȡ�̳߳��е�ǰ�̵߳��ܸ���
	int GetTotalThreadSize() const { return this->worker_threads_.size(); }

	// ��ȡ��ǰ�̳߳��Ѿ�ִ�й��ĺ�������
	int GetRunnedFuncNum() const { return total_function_num_.load(); }

	// ��ǰ�̳߳��Ƿ����
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

	//std::function��һ��ͨ�á���̬�ĺ�����װ
	std::queue<std::function<void()>> tasks_;//�������
	std::mutex task_mtx_;//��ס�������
	std::condition_variable cv_;//�ź���

	std::atomic<int> waiting_thread_num_;
	std::atomic<int> total_function_num_;

	std::atomic_bool is_shudown_;
	std::atomic_bool is_shutdown_now_;
	std::atomic<bool> is_available_;
};