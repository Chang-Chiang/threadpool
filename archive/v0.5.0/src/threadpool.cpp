#include "threadpool.h"

#include <functional>
#include <iostream>
#include <thread>

const int THREAD_MAX_THRESHHOLD = 10;      // 允许创建最大线程数
const int TASK_MAX_THRESHHOLD = INT32_MAX; // 任务队列允许最大任务数
const int THREAD_MAX_IDLE_TIME = 10;       // 线程最大空闲时间 单位：秒

ThreadPool::ThreadPool()
    : initThreadSize_(0)
    , taskSize_(0)
    , idleThreadSize_(0)
    , curThreadSize_(0)
    , taskQueMaxThreshHold_(TASK_MAX_THRESHHOLD)
    , threadSizeThreshHold_(THREAD_MAX_THRESHHOLD)
    , poolMode_(PoolMode::MODE_FIXED)
    , isPoolRunning_(false) {}

ThreadPool::~ThreadPool() {}

void ThreadPool::setMode(PoolMode mode) {

    // 禁止线程池启动后再去修改线程池工作模式
    if (checkRunningState()) {
        return;
    }
    poolMode_ = mode;
}

void ThreadPool::setTaskQueMaxThreshHold(int threshhold) {

    // 禁止线程池启动后再去修改任务队列最大任务数
    if (checkRunningState()) {
        return;
    }
    taskQueMaxThreshHold_ = threshhold;
}

void ThreadPool::setThreadSizeThreshHold(int threshhold) {

    // 禁止线程池启动后再去修改线程最大数量
    if (checkRunningState()) {
        return;
    }
    if (poolMode_ == PoolMode::MODE_CACHED) {
        threadSizeThreshHold_ = threshhold;
    }
}

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

    /** cached 模式
     * 适用场景：任务处理比较紧急，小而快的任务
     * 不适用耗时的任务，长时间占用线程，导致系统线程创建过多
     * 比较根据任务数量和空闲线程数量，判断是否需要创建新的线程
     */

    // 线程池工作在 cached 模式 && 任务数量大于空闲线程数量 &&
    // 当前线程数量小于系统允许创建的最大线程数
    if (poolMode_ == PoolMode::MODE_CACHED && taskSize_ > idleThreadSize_ &&
        curThreadSize_ < threadSizeThreshHold_) {

        std::cout << ">>> create new thread..." << std::endl;

        // 创建新线程
        std::unique_ptr<Thread> ptr = std::make_unique<Thread>(
            std::bind(&ThreadPool::threadFunc, this, std::placeholders::_1));
        int threadId = ptr->getId();
        threads_.emplace(threadId, std::move(ptr));
        threads_[threadId]->start();
        curThreadSize_++;
        idleThreadSize_++;
    }
    // 返回任务的 Result 对象
    return Result(sp);
    // 不能使用这种设计
    // 线程执行完任务后，任务便从任务队列中 pop 析构
    // task 析构，而这种方式 Result 类对象依赖于 task 对象
    // return task->getResult();
}

void ThreadPool::start(int initThreadSize) {

    // 设置线程池的运行状态
    isPoolRunning_ = true;

    // 记录初始线程个数
    initThreadSize_ = initThreadSize;

    curThreadSize_ = initThreadSize;

    // 创建线程对象
    for (int i = 0; i < initThreadSize_; i++) {

        // 把线程函数绑定到 thread 线程对象

        std::unique_ptr<Thread> ptr = std::make_unique<Thread>(
            std::bind(&ThreadPool::threadFunc, this, std::placeholders::_1));
        int threadId = ptr->getId();
        threads_.emplace(threadId, std::move(ptr));
    }

    // 启动所有线程
    for (int i = 0; i < initThreadSize_; i++) {

        // 需要去执行一个线程函数
        threads_[i]->start();

        // 记录初始空闲线程数量，每启动一个线程就更新空闲线程数
        idleThreadSize_++;
    }
}

void ThreadPool::threadFunc(int threadId) {

    auto lastTime = std::chrono::high_resolution_clock().now();

    // 线程不断循环，从任务队列取任务
    for (;;) {

        std::shared_ptr<Task> task;

        // 添加作用域，使线程取到任务后出作用域即释放锁
        {
            // 【线程互斥】获取锁
            std::unique_lock<std::mutex> lock(taskQueMtx_);

            std::cout << "tid: " << std::this_thread::get_id() << " 尝试获取任务..." << std::endl;

            // cached 模式，可能创建了很多线程
            // 但线程空闲时间超过 60s，多余线程结束回收（超过 initThreadSize_ 数量的）
            if (poolMode_ == PoolMode::MODE_CACHED) {

                /* 每 1 s 返回一次 区分：超时返回 or 有任务待执行返回 */

                while (taskQue_.size() == 0) {

                    // 超时返回
                    if (std::cv_status::timeout ==
                        notEmpty_.wait_for(lock, std::chrono::seconds(1))) {
                        auto now = std::chrono::high_resolution_clock().now();
                        auto dur = std::chrono::duration_cast<std::chrono::seconds>(now - lastTime);

                        if (dur.count() >= THREAD_MAX_IDLE_TIME &&
                            curThreadSize_ > initThreadSize_) {
                            /* 线程空闲超时，开始回收 */

                            // 把线程对象从线程列表容器中删除
                            // 根据线程 id，实现线程函数 ThreadFun 与线程对象 Thread 的对应关系
                            threads_.erase(threadId);
                            // 修改记录线程数量的相关变量的值
                            curThreadSize_--;
                            idleThreadSize_--;
                            std::cout << "tid " << std::this_thread::get_id() << " exit!"
                                      << std::endl;
                            return;
                        }
                    }
                }
            }

            // 【线程通信】等待 notEmpty_ 任务队列非空
            notEmpty_.wait(lock, [&]() -> bool { return taskQue_.size() > 0; });
            // 每取一个任务，更新空闲线程数
            idleThreadSize_--;

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

        // 线程函数执行结束，更新空闲线程数
        idleThreadSize_++;

        // 更新线程执行完任务的时间
        lastTime = std::chrono::high_resolution_clock().now();
    }
}

bool ThreadPool::checkRunningState() const { return isPoolRunning_; }