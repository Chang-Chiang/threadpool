#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue> // 用于存放任务队列
#include <thread>
// #include <vector> // 用于存放线程池
#include <unordered_map> // 用于存放线程池，实现线程与线程 id 的对应关系

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
    // std::vector<std::unique_ptr<Thread>> threads_;              // 线程列表
    std::unordered_map<int, std::unique_ptr<Thread>> threads_;

    int             initThreadSize_;       // 初始的线程数量
    int             threadSizeThreshHold_; // 线程池最大线程数
    std::atomic_int curThreadSize_;        // 当前线程池中线程总数量
    std::atomic_int idleThreadSize_;       // 记录空闲线程数量

    // 用裸指针存在问题：用户传递临时对象，析构后则会出现问题
    std::queue<std::shared_ptr<Task>> taskQue_; // 任务队列

    // 提交任务时，计数自增；线程从任务队列取任务，计数自减
    // 提交任务线程 与 线程函数线程 不在一个线程中，考虑线程安全定义为 atomic_int
    std::atomic_int taskSize_;           // 任务队列中任务计数

    int taskQueMaxThreshHold_;           // 任务队列最大任务数

    std::mutex              taskQueMtx_; // 互斥锁，保证任务队列线程安全
    std::condition_variable notFull_;    // 条件变量，表示任务队列未满
    std::condition_variable notEmpty_;   // 条件变量，表示任务队列非空
    std::condition_variable exitCond_;   // 等待线程资源全部回收

    PoolMode         poolMode_;          // 当前线程池的工作模式
    std::atomic_bool isPoolRunning_;     // 表示当前线程池的启动状态
                                         // 保证线程池相关设置操作在线程池启动前
                                         // 多个线程都会使用到，定义为 atomic

private:
    // 线程函数
    void threadFunc(int threadId);

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

    // 设置线程池 cached 模式下线程最大数量
    void setThreadSizeThreshHold(int threshhold);

    // 给线程池提交任务
    Result submitTask(std::shared_ptr<Task> sp);

    // 开启线程池
    void start(int initThreadSize = 4);
};

#endif