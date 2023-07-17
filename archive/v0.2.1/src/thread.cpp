#include "thread.h"

#include <thread>

Thread::Thread(ThreadFunc func) : func_(func) {}

Thread::~Thread() {}

void Thread::start() {

    // 创建一个线程来执行一个线程函数，pthread_create
    std::thread t(func_);

    // 出作用域线程对象 t 析构，但线程函数 func_ 还存在
    // 所以将 t 设置为分离线程
    t.detach();
}