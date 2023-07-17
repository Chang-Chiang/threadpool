/**
 * @file task.cpp
 * @author Chang Chaing (you@domain.com)
 * @brief 任务抽象基类实现
 * @version 0.1
 * @date 2023-05-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "task.h"

#include "result.h"

Task::Task() : result_(nullptr) {}

void Task::exec() {
    if (result_ != nullptr) {

        // 此处发生多态调用
        result_->setVal(run());
    }
}

void Task::setResult(Result* res) { result_ = res; }
