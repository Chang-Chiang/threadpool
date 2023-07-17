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
