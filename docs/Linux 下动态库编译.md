```bash
$ g++ -fPIC -shared *.cpp -o libthreadpool.so -std=c++17

在 /usr/lib /usr/local/lib 找 .a .so 库文件
在 /usr/include /usr/local/include 找 .h 头文件
$ mv libthreadpool.so /usr/local/lib
$ mv threadpool.h /usr/local/include
# 编译时找库如上述路径
$ g++ 线程池测试.cpp -std=c++17 -lthreadpool -lpthread  

$ ./a.out
error while loading shared libraries...

# 运行时找库路径 /etc/ld.so.conf 文件
# /etc/ld.so.conf.d 路径下很多 .conf 文件
# 在该路径下创建一个 .conf 文件 如 mylib.conf
# 在该文件写入 /usr/local/lib
# 执行 ldconfig 配置文件刷新到 ld.so.cache 运行时动态库信息
$ ldconfig

```

linux 平台结束不了

```bash
$ ps -u  # 查看进程
$ gdb attach 6359  # 调试正在运行的进程 
$ info threads
$ bt  # 打印线程堆栈
$ thread 5
$ bt

$ find / -name condition_variable
```

