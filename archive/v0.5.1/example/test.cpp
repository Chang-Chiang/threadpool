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
        std::cout << "tid:" << std::this_thread::get_id() << " begin!" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        uLong sum = 0;
        for (uLong i = begin_; i <= end_; i++) {
            sum += i;
        }
        std::cout << "tid:" << std::this_thread::get_id() << " end!" << std::endl;

        return sum;
    }

private:
    int begin_;
    int end_;
};

int main() {
    /**
     * 4 个线程被 4 个任务占满
     * 又新增 2 个任务，又创建 2 个新的线程
     * 超过最大线程空闲时间，回收新创建的 2 个线程
     */

    ThreadPool pool;
    // 用户自行设置线程池工作模式
    pool.setMode(PoolMode::MODE_CACHED);
    // 启动线程池
    pool.start(4);

    Result res1 = pool.submitTask(std::make_shared<MyTask>(1, 100000000));
    Result res2 = pool.submitTask(std::make_shared<MyTask>(100000001, 200000000));
    Result res3 = pool.submitTask(std::make_shared<MyTask>(200000001, 300000000));
    Result res4 = pool.submitTask(std::make_shared<MyTask>(300000001, 400000000));
    Result res5 = pool.submitTask(std::make_shared<MyTask>(400000001, 500000000));
    Result res6 = pool.submitTask(std::make_shared<MyTask>(500000001, 600000000));

    uLong sum1 = res1.get().cast_<uLong>();
    uLong sum2 = res2.get().cast_<uLong>();
    uLong sum3 = res3.get().cast_<uLong>();

    std::cout << (sum1 + sum2 + sum3) << std::endl;

    getchar();
}