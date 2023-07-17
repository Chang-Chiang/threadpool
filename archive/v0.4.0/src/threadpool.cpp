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

Result ThreadPool::submitTask(std::shared_ptr<Task> sp) {
    // 【线程互斥】获取锁
    std::unique_lock<std::mutex> lock(taskQueMtx_);

    // 【线程通信】等待任务队列有空余，用户提交任务阻塞不超过 1s
    if (!notFull_.wait_for(lock, std::chrono::seconds(1), [&]() -> bool {
            return taskQue_.size() < (size_t)taskQueMaxThreshHold_;
        })) {
        // notFull_ 等待 1s，任务队列仍满，提交任务失败
        std::cerr << "task queue is full, submit task failed." << std::endl;
        return Result(sp, false);
    }

    // 【线程通信】如果任务队列未满，把任务放入任务队列
    taskQue_.emplace(sp);
    taskSize_++;

    // 【线程通信】新放任务后，任务队列非空，通知消费者线程池分配线程执行任务
    notEmpty_.notify_all();

    // 返回任务的 Result 对象
    return Result(sp);

    // 不能使用这种设计
    // 线程执行完任务后，任务便从任务队列中 pop 析构
    // task 析构，而这种方式 Result 类对象依赖于 task 对象
    // return task->getResult();
}

void ThreadPool::start(int initThreadSize) {

    // 记录初始线程个数
    initThreadSize_ = initThreadSize;

    // 创建线程对象
    for (int i = 0; i < initThreadSize_; i++) {

        // 把线程函数绑定到 thread 线程对象

        std::unique_ptr<Thread> ptr =
            std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc, this));
        threads_.emplace_back(std::move(ptr));
    }

    // 启动所有线程
    for (int i = 0; i < initThreadSize_; i++) {

        // 需要去执行一个线程函数
        threads_[i]->start();
    }
}

void ThreadPool::threadFunc() {
    // 线程不断循环，从任务队列取任务
    for (;;) {

        std::shared_ptr<Task> task;

        // 添加作用域，使线程取到任务后出作用域即释放锁
        {
            // 【线程互斥】获取锁
            std::unique_lock<std::mutex> lock(taskQueMtx_);

            std::cout << "tid: " << std::this_thread::get_id() << " 尝试获取任务..." << std::endl;

            // 【线程通信】等待 notEmpty_ 任务队列非空
            notEmpty_.wait(lock, [&]() -> bool { return taskQue_.size() > 0; });

            std::cout << "tid: " << std::this_thread::get_id() << " 获取任务成功..." << std::endl;

            // 从任务队列取一个任务
            task = taskQue_.front();
            taskQue_.pop();
            taskSize_--;

            // 取出任务后，任务队列还有任务，非空，通知其他线程继续取任务（通知消费）
            if (taskQue_.size() > 0) {
                notEmpty_.notify_all();
            }

            // 取出任务后，任务队列未满，通知继续提交任务（通知生产）
            notFull_.notify_all();
        }

        // 当前线程执行任务
        if (task != nullptr) {
            // 做两件事情：执行任务；把任务的返回值通过 Result 类的 setVal 方法给到 Result
            task->exec();
        }
    }
}