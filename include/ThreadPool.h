#pragma once
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
private:
	std::vector<std::thread> threads;
	std::queue<std::function<void()>> tasks;
	std::mutex tasks_mtx;
	std::condition_variable cv;
	bool stop;

public:
	ThreadPool(int size = 8);
	~ThreadPool();

	// C++编译器不支持模板分离编译，因此add在.h文件内定义
	template <class F, class... Args>
	auto add(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>;
};

template <class F, class... Args>
auto ThreadPool::add(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type> {
	using return_type = typename std::result_of<F(Args...)>::type;

	// 将任务F打包为异步任务packaged_task，并用shared_ptr包装
	auto task = std::make_shared<std::packaged_task<return_type()>>(
		// 返回一个F对象，其参数绑定为Args
		std::bind(std::forward<F>(f), std::forward<Args>(args)...));

	std::future<return_type> res = task->get_future();
	{
		std::unique_lock<std::mutex> lock(tasks_mtx);  // 加锁，作用域为大括号内

		if (stop) throw std::runtime_error("enqueue on stopped ThreadPool");

		tasks.emplace([task]() { (*task)(); });
	}
	cv.notify_one();
	return res;
}