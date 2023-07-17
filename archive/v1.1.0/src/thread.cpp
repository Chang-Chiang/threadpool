#include "thread.h"

#include <thread>

int Thread::generateId_ = 0;

Thread::Thread(ThreadFunc func) : func_(func), threadId_(generateId_++) {}

Thread::~Thread() {}

void Thread::start() {

    // 创建一个线程来执行一个线程函数 pthread_create
    std::thread t(func_, threadId_);

    // 设置分离线程
    t.detach();
}

int Thread::getId() const { return threadId_; }