#pragma once

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <thread>
// #include <vector>

#include "result.h"
#include "task.h"
#include "thread.h"

// 线程池支持的模式
// 防止 enum PoolMode {} 写法导致枚举量与其他枚举结构中的枚举量冲突
enum class PoolMode {
    MODE_FIXED,  // 固定数量的线程
    MODE_CACHED, // 线程数量可动态增长
};

// 线程池类
class ThreadPool {

private:
    // std::vector<Thread*> threads_; // 线程列表，vector 内放裸指针需手动释放，优化为智能指针
    // std::vector<std::unique_ptr<Thread>>             threads_; // 线程列表
    std::unordered_map<int, std::unique_ptr<Thread>> threads_; // 线程列表
    // size_t               initThreadSize_;  // 初始的线程数量线程

    int             initThreadSize_;       // 初始的线程数量
    int             threadSizeThreshHold_; // 线程数量上限阈值
    std::atomic_int curThreadSize_;        // 记录当前线程池里面线程的总数量
    std::atomic_int idleThreadSize_;       // 记录空闲线程的数量

    // 用裸指针存在问题：用户传递临时对象，析构后则出现问题
    std::queue<std::shared_ptr<Task>> taskQue_;              // 任务队列
    std::atomic_int                   taskSize_;             // 任务队列中任务计数
    int                               taskQueMaxThreshHold_; // 任务队列最大任务数

    std::mutex              taskQueMtx_; // 互斥锁，保证任务队列线程安全
    std::condition_variable notFull_;    // 条件变量，任务队列未满
    std::condition_variable notEmpty_;   // 条件变量，任务队列非空
    std::condition_variable exitCond_;   // 等待线程资源全部回收

    PoolMode         poolMode_;          // 当前线程池的工作模式
    std::atomic_bool isPoolRunning_;     // 表示当前线程池的启动状态

private:
    // 线程函数
    void threadFunc(int threadid);

    // 检查线程池运行状态
    bool checkRunningState() const;

public:
    // 线程池构造函数
    ThreadPool();

    // 线程池析构函数
    ~ThreadPool();

    // 禁止拷贝构造
    ThreadPool(const ThreadPool&) = delete;

    // 禁止赋值运算符重载
    ThreadPool& operator=(const ThreadPool&) = delete;

public:
    // 设置线程池工作模式
    void setMode(PoolMode mode);

    // 设置 task 任务队列最大任务数
    void setTaskQueMaxThreshHold(int threshhold);

    // 线程池 cached 模式下线程队列最大线程数
    void setThreadSizeThreshHold(int threshhold);

    // 给线程池提交任务
    Result submitTask(std::shared_ptr<Task> sp);

    // 开启线程池
    // void start(int initThreadSize = 4);
    void start(int initThreadSize = std::thread::hardware_concurrency());
};