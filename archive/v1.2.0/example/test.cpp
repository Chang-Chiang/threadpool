/**
 * @file test.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-06-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <chrono>
#include <future>
#include <iostream>
#include <thread>

#include "../src/threadpool.hpp"

int sum1(int a, int b) {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // 比较耗时
    return a + b;
}

int sum2(int a, int b, int c) {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return a + b + c;
}

// io线程
void io_thread(int listenfd) {}

// worker线程
void worker_thread(int clientfd) {}

int main() {
    ThreadPool pool;
    // pool.setMode(PoolMode::MODE_CACHED);
    pool.start(2);

    std::future<int> r1 = pool.submitTask(sum1, 1, 2);
    std::future<int> r2 = pool.submitTask(sum2, 1, 2, 3);
    std::future<int> r3 = pool.submitTask(
        [](int b, int e) -> int {
            int sum = 0;
            for (int i = b; i <= e; i++) {
                sum += i;
            }
            return sum;
        },
        1, 100);
    std::future<int> r4 = pool.submitTask(
        [](int b, int e) -> int {
            int sum = 0;
            for (int i = b; i <= e; i++) {
                sum += i;
            }
            return sum;
        },
        1, 100);
    std::future<int> r5 = pool.submitTask(
        [](int b, int e) -> int {
            int sum = 0;
            for (int i = b; i <= e; i++) {
                sum += i;
            }
            return sum;
        },
        1, 100);

    std::cout << r1.get() << std::endl;
    std::cout << r2.get() << std::endl;
    std::cout << r3.get() << std::endl;
    std::cout << r4.get() << std::endl;
    std::cout << r5.get() << std::endl;
}