#include "ThreadPool.h"
#include <thread>
#include <functional>
#include <mutex>
#include <queue>
#include <vector>
#include <condition_variable>

ThreadPool::ThreadPool(int size):stop(false) {
    for(int i = 0; i < size; ++i) {
        threads.emplace_back(std::thread([this]() {
            while(true) { 
                std::function<void()> task;
                {
                    // printf("1\n");
                    // 通过lock变量将mtx保护封装起来，上下大括号为声明的锁作用域，出了作用域自动unlock
                    std::unique_lock<std::mutex> lock(tasks_mtx);
                    // 如果已上锁或stop或任务队列空，阻塞
                    cv.wait(lock, [this]() {
                        return stop || !tasks.empty();
                    });
                    if(stop && tasks.empty()) return;
                    task = tasks.front();
                    tasks.pop();
                }
                task();
            }
        }));
    }
}

ThreadPool::~ThreadPool() {
    {
        // 将stop设为1，说明线程池即将销毁
        std::unique_lock<std::mutex> lock(tasks_mtx);
        stop = true;
    }
    // 通知所有线程
    cv.notify_all();
    for(std::thread &th : threads) {
        // 等待该进程销毁
        if(th.joinable()) th.join();
    }
}
