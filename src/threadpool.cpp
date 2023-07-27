#include "threadpool.h"

#include <functional>
#include <iostream>
#include <thread>

const int THREAD_MAX_THRESHHOLD = 100;
const int TASK_MAX_THRESHHOLD = INT32_MAX;
const int THREAD_MAX_IDLE_TIME = 60; // 单位：秒

ThreadPool::ThreadPool()
    : initThreadSize_(0)
    , taskSize_(0)
    , idleThreadSize_(0)
    , curThreadSize_(0)
    , taskQueMaxThreshHold_(TASK_MAX_THRESHHOLD)
    , threadSizeThreshHold_(THREAD_MAX_THRESHHOLD)
    , poolMode_(PoolMode::MODE_FIXED)
    , isPoolRunning_(false) {}

ThreadPool::~ThreadPool() {
    isPoolRunning_ = false;
    // notEmpty_.notify_all();  // 写在锁的上面会造成死锁问题！

    // 等待线程池里面所有的线程返回
    // 线程池里面的线程有两种状态：阻塞 & 正在执行任务中

    std::unique_lock<std::mutex> lock(taskQueMtx_);
    notEmpty_.notify_all();
    exitCond_.wait(lock, [&]() -> bool { return threads_.size() == 0; });
}

void ThreadPool::setMode(PoolMode mode) {
    if (checkRunningState()) {
        return;
    }
    poolMode_ = mode;
}

void ThreadPool::setTaskQueMaxThreshHold(int threshhold) {
    if (checkRunningState()) {
        return;
    }
    taskQueMaxThreshHold_ = threshhold;
}

void ThreadPool::setThreadSizeThreshHold(int threshhold) {
    if (checkRunningState()) {
        return;
    }
    if (poolMode_ == PoolMode::MODE_CACHED) {
        threadSizeThreshHold_ = threshhold;
    }
}

Result ThreadPool::submitTask(std::shared_ptr<Task> sp) {
    // 获取锁
    std::unique_lock<std::mutex> lock(taskQueMtx_);

    // 线程的通信，等待任务队列有空余

    // 用户提交任务，最长不能阻塞超过 1s，否则判断提交任务失败，返回
    if (!notFull_.wait_for(lock, std::chrono::seconds(1), [&]() -> bool {
            return taskQue_.size() < (size_t)taskQueMaxThreshHold_;
        })) {

        // 表示 notFull_ 等待 1s，条件依然没有满足
        std::cerr << "task queue is full, submit task fail." << std::endl;
        // return task->getResult();  // Task  Result   线程执行完task，task对象就被析构掉了
        return Result(sp, false);
    }

    // 如果有空余，把任务放入任务队列中
    taskQue_.emplace(sp);
    taskSize_++;

    // 新放任务后，在 notEmpty_ 上进行通知，分配线程执行任务
    notEmpty_.notify_all();

    // cached模式 任务处理比较紧急 场景：小而快的任务
    // 需要根据任务数量和空闲线程的数量，判断是否需要创建新的线程出来
    if (poolMode_ == PoolMode::MODE_CACHED && taskSize_ > idleThreadSize_ &&
        curThreadSize_ < threadSizeThreshHold_) {
        std::cout << ">>> create new thread..." << std::endl;

        // 创建新的线程对象
        auto ptr = std::make_unique<Thread>(
            std::bind(&ThreadPool::threadFunc, this, std::placeholders::_1));
        int threadId = ptr->getId();
        threads_.emplace(threadId, std::move(ptr));
        // 启动线程
        threads_[threadId]->start();
        // 修改线程个数相关的变量
        curThreadSize_++;
        idleThreadSize_++;
    }

    // 返回任务的Result对象
    return Result(sp);
    // return task->getResult(); // 不能使用
}

void ThreadPool::start(int initThreadSize) {
    // 设置线程池的运行状态
    isPoolRunning_ = true;

    // 记录初始线程个数
    initThreadSize_ = initThreadSize;
    curThreadSize_ = initThreadSize;

    // 创建线程对象
    for (int i = 0; i < initThreadSize_; i++) {
        // 把线程函数给到 thread 线程对象
        // auto = std::unique_ptr<Thread>
        // C++14, make_unique
        auto ptr = std::make_unique<Thread>(
            std::bind(&ThreadPool::threadFunc, this, std::placeholders::_1));
        int threadId = ptr->getId();
        threads_.emplace(threadId, std::move(ptr));
        // threads_.emplace_back(std::move(ptr));
    }

    // 启动所有线程  std::vector<Thread*> threads_;
    for (int i = 0; i < initThreadSize_; i++) {
        threads_[i]->start(); // 需要去执行一个线程函数
        idleThreadSize_++;    // 记录初始空闲线程的数量
    }
}

void ThreadPool::threadFunc(int threadid) {
    auto lastTime = std::chrono::high_resolution_clock().now();

    // 所有任务必须执行完成，线程池才可以回收所有线程资源
    for (;;) {
        std::shared_ptr<Task> task;

        // 加作用域，保证线程池取出任务后释放锁，允许其他线程取任务
        {
            // 先获取锁
            std::unique_lock<std::mutex> lock(taskQueMtx_);

            std::cout << "tid:" << std::this_thread::get_id() << " 尝试获取任务..." << std::endl;

            // cached模式下，有可能已经创建了很多的线程，但是空闲时间超过60s，应该把多余的线程
            // 结束回收掉（超过initThreadSize_数量的线程要进行回收）
            // 当前时间 - 上一次线程执行的时间 > 60s

            // 每一秒中返回一次  怎么区分：超时返回？还是有任务待执行返回
            // 锁 + 双重判断
            while (taskQue_.size() == 0) {
                // 线程池要结束，回收线程资源
                if (!isPoolRunning_) {
                    threads_.erase(threadid); // std::this_thread::getid()
                    std::cout << "threadid:" << std::this_thread::get_id() << " exit!" << std::endl;
                    exitCond_.notify_all();
                    return; // 线程函数结束，线程结束
                }

                if (poolMode_ == PoolMode::MODE_CACHED) {
                    // 条件变量，超时返回
                    if (std::cv_status::timeout ==
                        notEmpty_.wait_for(lock, std::chrono::seconds(1))) {
                        auto now = std::chrono::high_resolution_clock().now();
                        auto dur = std::chrono::duration_cast<std::chrono::seconds>(now - lastTime);
                        if (dur.count() >= THREAD_MAX_IDLE_TIME &&
                            curThreadSize_ > initThreadSize_) {
                            // 开始回收当前线程
                            // 记录线程数量的相关变量的值修改
                            // 把线程对象从线程列表容器中删除   没有办法将 threadFunc 与 thread对象
                            // 匹配 通过 threadid 找到 thread对象 进而将其删除
                            threads_.erase(threadid); // 不要传入 std::this_thread::getid()
                            curThreadSize_--;
                            idleThreadSize_--;

                            std::cout << "threadid:" << std::this_thread::get_id() << " exit!"
                                      << std::endl;
                            return;
                        }
                    }
                }
                else {
                    // 等待 notEmpty 条件
                    notEmpty_.wait(lock);
                }
            }

            idleThreadSize_--;

            std::cout << "tid:" << std::this_thread::get_id() << " 获取任务成功..." << std::endl;

            // 从任务队列取一个任务
            task = taskQue_.front();
            taskQue_.pop();
            taskSize_--;

            // 如果依然有剩余任务，继续通知其它得线程执行任务
            if (taskQue_.size() > 0) {
                notEmpty_.notify_all();
            }

            // 取出一个任务后，通知可以继续提交生产任务
            notFull_.notify_all();
        }

        // 当前线程负责执行这个任务
        if (task != nullptr) {
            // task->run(); // 执行任务；把任务的返回值setVal方法给到Result
            task->exec();
        }

        idleThreadSize_++;
        lastTime = std::chrono::high_resolution_clock().now(); // 更新线程执行完任务的时间
    }
}

bool ThreadPool::checkRunningState() const { return isPoolRunning_; }