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

class Task {
public:
    // 用户可以自定义任意类型，从 Task 类继承，重写类方法，实现自定义任务处理
    virtual void run() = 0;
};
#endif