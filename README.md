# threadpool
## Table of Contents

+ [Background](#Background)
+ [Install](#Install)
+ [Usage](#Usage)
+ [Docs](#Docs)

## Background

Creating and destroying threads are both very "heavy" operations, which are time-consuming and performance intensive. Therefore, during the process of service execution, if the transaction volume is large and threads are created, executed, and destroyed in real-time after the transaction is completed,  it will lead to a decrease in the real-time performance of the system.

The advantage of the thread pool is that the threads in the thread pool are created in advance at the beginning of the service process startup. When business traffic arrives, threads just need to be allocated. You can directly obtain an idle thread from the thread pool to execute the task. After the task is executed, you do not need to release the thread, but return the thread to the thread pool to continue to provide services for subsequent tasks.

## Install

### Requirements

### Clone

Clone the repository to your local environment:

```bash
$ git clone https://github.com/Chang-Chiang/threadpool.git
```

### Compile

```bash
$ chmod +x autobuild.sh
$ sudo ./autobuild.sh
```

### Uninstall

```bash
# 如果编译的静态库安装在系统目录下则卸载时将其删除
$ rm 
```

## Usage

```c++
// write your task class 
class MyTask : public Task {
public:
    MyTask(int begin, int end) : begin_(begin), end_(end) {}

    // here to run your task function
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

 
ThreadPool pool;  // create threadpool object
pool.setMode(PoolMode::MODE_CACHED);  // set the mode

// submit your task to threadpool and get the result
Result res1 = pool.submitTask(std::make_shared<MyTask>(1, 100000000));  
```

## Docs

+ TODO
