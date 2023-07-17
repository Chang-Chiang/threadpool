#include "threadpool.h"

#include <functional>
#include <iostream>
#include <thread>

const int THREAD_MAX_THRESHHOLD = 100; // 允许创建最大线程数
const int TASK_MAX_THRESHHOLD = 1024;  // 任务队列允许最大任务数

ThreadPool::ThreadPool()
    : initThreadSize_(0)
    , taskSize_(0)
    , taskQueMaxThreshHold_(TASK_MAX_THRESHHOLD)
    , poolMode_(PoolMode::MODE_FIXED) {}

ThreadPool::~ThreadPool() {}

void ThreadPool::setMode(PoolMode mode) { poolMode_ = mode; }

void ThreadPool::setTaskQueMaxThreshHold(int threshhold) { taskQueMaxThreshHold_ = threshhold; }

void ThreadPool::submitTask(std::shared_ptr<Task> sp) {}

void ThreadPool::start(int initThreadSize) {

    // 记录初始线程个数
    initThreadSize_ = initThreadSize;

    // 创建线程对象
    for (int i = 0; i < initThreadSize_; i++) {

        // 把线程函数绑定到 thread 线程对象

        // 可用 auto 替代 std::unique_ptr<Thread>
        // make_share 是 C++11
        // make_unique 是 C++14
        std::unique_ptr<Thread> ptr =
            std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc, this));
        // threads_.emplace_back(ptr);  // 会在容器底层拷贝构造，但 unique_ptr 不允许拷贝构造
        threads_.emplace_back(std::move(ptr));
    }

    // 启动所有线程
    for (int i = 0; i < initThreadSize_; i++) {

        // 需要去执行一个线程函数
        threads_[i]->start();
    }
}

void ThreadPool::threadFunc() {
    std::cout << "begin threadFunc [tid]: " << std::this_thread::get_id() << std::endl;
    std::cout << "end threadFunc [tid]: " << std::this_thread::get_id() << std::endl;
}