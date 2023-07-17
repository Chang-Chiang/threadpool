/**
 * @file sem.h
 * @author Chang Chiang (Chang_Chiang@outlook.com)
 * @brief mutex 实现信号量(C++20 提供语言层面的信号量)
 * @version 0.1
 * @date 2023-05-27
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef SEM_H
#define SEM_H

#include <condition_variable>
#include <memory>
#include <mutex>

class Semaphore {
private:
    int                     resLimit_; // 资源计数
    std::mutex              mtx_;      // 互斥锁
    std::condition_variable cond_;     // 条件变量

public:
    Semaphore(int limit = 0);
    ~Semaphore() = default;

public:
    // 获取一个信号量资源
    void wait();

    // 增加一个信号量资源
    void post();
};
#endif