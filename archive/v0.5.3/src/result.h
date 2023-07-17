/**
 * @file result.h
 * @author Chang Chiang (Chang_Chiang@outlook.com)
 * @brief 线程池任务返回值类定义
 * @version 0.1
 * @date 2023-05-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef RESULT_H
#define RESULT_H

#include <atomic>
#include <memory>

#include "any.hpp"
#include "sem.h"

class Task;

class Result {
public:
    Result(std::shared_ptr<Task> task, bool isValid = true);
    ~Result() = default;

    // 获取任务执行完的返回值
    void setVal(Any any);

    // 用户调用，获取 task 的返回值
    Any get();

private:
    Any                   any_;     // 存储任务的返回值
    Semaphore             sem_;     // 线程通信信号量
    std::shared_ptr<Task> task_;    // 指向对应获取返回值的任务对象
    std::atomic_bool      isValid_; // 返回值是否有效
};
#endif