# ChangeLog

## v0.1.0

+ 线程池相关组件类定义与实现：
  + `class ThreadPool {};` 线程池类 
  + `class Thread {};` 线程池中存放的线程类 
  + `class Task {};`  用于提供用户传入任意任务的抽象基类
  + `enum class PoolMode {};` 线程池支持的两种模式：
    + 固定数量
    + 线程数量可动态增长

## v0.1.1

+ :bug:：线程池容器存放元素，由裸指针修改为智能指针`unique_ptr`，对应修改线程创建部分

## v0.2.0

+ 用户向线程池提交任务接口实现

## v0.2.1

+ 限制用户提交任务阻塞时间

## v0.3.0

+ 线程池线程函数接口实现

## v0.4.0

+ 获取线程执行任务的返回值
  + `class Task {};` 中的 `run()` 方法返回值，表示任意类型
  + `class Result {};` 返回值设计
+ `class Any {};` 所有类型的基类，类似于 java 和 python 里的 Object，用于接收任意类型
+ `class Semaphore {};` 信号量实现

## v0.5.0

+ 线程池 Cached 模式设计：适合小而快的任务。不适合耗时的任务，长时间占用线程，导致系统线程创建过多
  + 用户设置线程池工作模式
  + 提交任务时根据任务数量和空闲线程数量，判断是否需要创建新的线程
  + 线程池执行时，cached 模式下，可能创建了过多的线程，在一定空闲时间后（eg. 60s），结束回收多余的线程

## v0.5.1

+ ThreadPool 析构后，回收线程池相关资源

## v0.5.2

+ 开启线程池时默认线程数为 CPU 核心数

## v0.5.3

+ :bug:：【死锁问题】死锁问题解决

## v0.5.4

+ 线程池 ThreadPool 出作用域析构时，此时任务队列里面如果还有任务，等任务全部执行完成，再结束

## v1.1.0

+ 动态库编译

+ :bug:：【跨平台死锁问题】Linux 环境条件变量未析构问题

## v1.2.0

+ 使用 C++11 替代 `class Result{};`