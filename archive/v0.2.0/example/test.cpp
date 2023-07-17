#include <chrono>
#include <iostream>
#include <thread>

#include "../src/threadpool.h"

int main() {
    ThreadPool pool;
    pool.start(6);

    // 主线程睡眠 5s
    std::this_thread::sleep_for(std::chrono::seconds(5));
}