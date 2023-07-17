#include <chrono>
#include <iostream>
#include <thread>

#include "../src/threadpool.h"

class MyTask : public Task {
public:
    void run() {
        std::cout << "tid: " << std::this_thread::get_id() << " begin!" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "tid: " << std::this_thread::get_id() << " end!" << std::endl;
    }
};

int main() {

    /**
     * 【测试一】
     * 4 个线程，3 个任务
     * 3 个线程抢到任务，1 个线程尝试获取任务但失败
     */
    // ThreadPool pool;
    // pool.start(4);
    // pool.submitTask(std::make_shared<MyTask>());
    // pool.submitTask(std::make_shared<MyTask>());
    // pool.submitTask(std::make_shared<MyTask>());

    /**
     * 【测试二】
     * 4 个线程，5 个任务
     * 4 个线程抢到任务，其中 1 个线程执行完后，抢第 5 个任务
     */
    ThreadPool pool;
    pool.start(4);
    pool.submitTask(std::make_shared<MyTask>());
    pool.submitTask(std::make_shared<MyTask>());
    pool.submitTask(std::make_shared<MyTask>());
    pool.submitTask(std::make_shared<MyTask>());
    pool.submitTask(std::make_shared<MyTask>());

    /**
     * 【测试三】
     * 每个线程执行任务时间较长
     * 后续提交任务时，任务队列已满，导致任务提交失败
     */

    getchar();
}