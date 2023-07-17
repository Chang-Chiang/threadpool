/**
 * @file task.h
 * @author Chang Chaing (you@domain.com)
 * @brief 任务抽象基类
 *
 * @version 0.1
 * @date 2023-05-28
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef TASK_H
#define TASK_H

class Result;
class Any;

class Task {
public:
    Task();
    ~Task() = default;
    void exec();
    void setResult(Result* res);

    // 用户可以自定义任意任务类型，从 Task 类继承，重写 run 方法，实现自定义任务处理
    virtual Any run() = 0;

private:
    // Result 对象生命周期长于 Task 对象，不用使用 shared_ptr
    // 使用裸指针而不是 shared_ptr，否则会与 Result 类中的 task_ 成员变量形成交叉引用问题
    Result* result_;
};
#endif