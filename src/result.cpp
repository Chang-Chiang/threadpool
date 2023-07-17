#include "result.h"

#include <memory>

#include "any.hpp"
#include "sem.hpp"
#include "task.h"

Result::Result(std::shared_ptr<Task> task, bool isValid) : isValid_(isValid), task_(task) {
    task_->setResult(this);
}

Any Result::get() {
    if (!isValid_) {
        return "";
    }

    // task 任务如果没有执行完，则阻塞用户的线程
    sem_.wait();

    return std::move(any_);
}

void Result::setVal(Any any) {
    // 存储 task 的返回值
    this->any_ = std::move(any);

    // 已经获取任务的返回值，增加信号量资源
    sem_.post();
}