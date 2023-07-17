#pragma once
#include <atomic>
#include <memory>

#include "any.hpp"
#include "sem.hpp"

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