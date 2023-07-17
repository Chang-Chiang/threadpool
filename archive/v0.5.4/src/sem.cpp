/**
 * @file sem.cpp
 * @author Chang Chiang (Chang_Chiang@outlook.com)
 * @brief 信号量实现
 * @version 0.1
 * @date 2023-05-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "sem.h"

Semaphore::Semaphore(int limit) : resLimit_(limit) {}

void Semaphore::wait() {
    std::unique_lock<std::mutex> lock(mtx_);

    // 等待信号量有资源，没有资源的话，会阻塞当前线程
    cond_.wait(lock, [&]() -> bool { return resLimit_ > 0; });

    resLimit_--;
}

void Semaphore::post() {
    std::unique_lock<std::mutex> lock(mtx_);
    resLimit_++;

    // 等待状态，释放 mutex 锁 通知条件变量 wait 的地方，可以起来干活了
    cond_.notify_all();
}