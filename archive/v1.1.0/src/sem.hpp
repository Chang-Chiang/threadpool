/**
 * @file sem.hpp
 * @author Chang Chiang (Chang_Chiang@outlook.com)
 * @brief mutex 实现信号量
 * @version 0.1
 * @date 2023-05-27
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef SEM_HPP
#define SEM_HPP

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>

class Semaphore {
private:
    std::atomic_bool        isExit_;
    int                     resLimit_;
    std::mutex              mtx_;
    std::condition_variable cond_;

public:
    Semaphore(int limit = 0) : resLimit_(limit), isExit_(false) {}
    ~Semaphore() { isExit_ = true; };

    // 获取一个信号量资源
    void wait() {
        if (isExit_) {
            return;
        }
        std::unique_lock<std::mutex> lock(mtx_);

        // 等待信号量有资源，没有资源的话，会阻塞当前线程
        cond_.wait(lock, [&]() -> bool { return resLimit_ > 0; });

        resLimit_--;
    }

    // 增加一个信号量资源
    void post() {
        if (isExit_) {
            return;
        }
        std::unique_lock<std::mutex> lock(mtx_);
        resLimit_++;
        // linux下 condition_variable 的析构函数什么也没做
        // 导致这里状态已经失效，无故阻塞
        cond_.notify_all(); // 等待状态，释放 mutex 锁 通知条件变量 wait 的地方，可以起来干活了
    }
};
#endif