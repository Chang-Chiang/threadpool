#include <chrono>
#include <iostream>
#include <thread>

#include "../src/threadpool.h"

using uLong = unsigned long long;

class MyTask : public Task {
public:
    MyTask(int begin, int end) : begin_(begin), end_(end) {}

    // run 方法最终就在线程池分配的线程中去做执行了
    Any run() {
        std::cout << "tid:" << std::this_thread::get_id() << "begin!" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        uLong sum = 0;
        for (uLong i = begin_; i <= end_; i++) {
            sum += i;
        }
        std::cout << "tid:" << std::this_thread::get_id() << "end!" << std::endl;

        return sum;
    }

private:
    int begin_;
    int end_;
};

int main() {
    /**
     * Master - Slave 线程模型
     *  Master 线程用来分解任务，然后给各个 Slave 线程分配任务
     * 等待各个 Slave 线程执行完任务，返回结果
     * Master 线程合并各个任务结果，输出
     */

    ThreadPool pool;
    pool.start(4);

    Result res1 = pool.submitTask(std::make_shared<MyTask>(1, 100000000));
    Result res2 = pool.submitTask(std::make_shared<MyTask>(100000001, 200000000));
    Result res3 = pool.submitTask(std::make_shared<MyTask>(200000001, 300000000));

    uLong sum1 = res1.get().cast_<uLong>();
    uLong sum2 = res2.get().cast_<uLong>();
    uLong sum3 = res3.get().cast_<uLong>();

    std::cout << (sum1 + sum2 + sum3) << std::endl;

    getchar();
}