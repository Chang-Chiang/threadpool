/**
 * @file thread.h
 * @author Chang Chiang (Chang_Chiang@outlook.com)
 * @brief 线程类定义
 * @version 0.1
 * @date 2023-05-27
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef THREAD_H
#define THREAD_H

#include <functional>

class Thread {

private:
    // 线程函数对象类型
    using ThreadFunc = std::function<void(int)>;

    ThreadFunc func_;

    static int generateId_;
    int        threadId_; // 保存线程 id

public:
    // 线程构造
    Thread(ThreadFunc func);

    // 线程析构
    ~Thread();

    // 启动线程
    void start();

    // 获取线程 id
    int getId() const;
};
#endif
