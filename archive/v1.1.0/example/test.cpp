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
#include <iostream>
#include <thread>

#include "../src/threadpool.h"

using uLong = unsigned long long;

class MyTask : public Task {
public:
    MyTask(int begin, int end) : begin_(begin), end_(end) {}
    // 问题一：怎么设计run函数的返回值，可以表示任意的类型
    // Java Python   Object 是所有其它类类型的基类
    // C++17 Any类型
    Any run() // run方法最终就在线程池分配的线程中去做执行了!
    {
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
    {
        ThreadPool pool;
        pool.setMode(PoolMode::MODE_CACHED);
        // 开始启动线程池
        pool.start(2);

        // linux上，这些Result对象也是局部对象，要析构的！！！
        Result res1 = pool.submitTask(std::make_shared<MyTask>(1, 100000000));
        Result res2 = pool.submitTask(std::make_shared<MyTask>(100000001, 200000000));
        pool.submitTask(std::make_shared<MyTask>(100000001, 200000000));
        pool.submitTask(std::make_shared<MyTask>(100000001, 200000000));
        pool.submitTask(std::make_shared<MyTask>(100000001, 200000000));

        // uLong sum1 = res1.get().cast_<uLong>();
        // cout << sum1 << endl;
    } // 这里 Result 对象也要析构!!! 信号量析构， 在vs下，条件变量析构会释放相应资源的

    std::cout << "main over!" << std::endl;
    getchar();
}