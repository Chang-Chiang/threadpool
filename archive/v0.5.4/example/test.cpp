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
     * 线程池 ThreadPool 出作用域析构时，此时任务队列里面如果还有任务
     * 选择一：等任务全部执行完成，再结束
     * 选择二：不执行剩下的任务
     * 设计成选择一
     */
    
    {
        ThreadPool pool;
        pool.setMode(PoolMode::MODE_CACHED);
        pool.start(2);

        Result res1 = pool.submitTask(std::make_shared<MyTask>(1, 100000000));
        Result res2 = pool.submitTask(std::make_shared<MyTask>(100000001, 200000000));

        pool.submitTask(std::make_shared<MyTask>(100000001, 200000000));
        pool.submitTask(std::make_shared<MyTask>(100000001, 200000000));
        pool.submitTask(std::make_shared<MyTask>(100000001, 200000000));

        uLong  sum1 = res1.get().cast_<uLong>();
        std::cout << sum1 << std::endl;
    }

    std::cout << "main over!" << std::endl;

    getchar();
}