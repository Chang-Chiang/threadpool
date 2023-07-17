#!/bin/bash

set -e


# 如果没有 build 目录, 创建该目录
if [ ! -d `pwd`/build ]; then
    mkdir `pwd`/build
fi

# 删除当前目录下的 build 目录所有文件
rm -rf `pwd`/build/*

# 切换至 build 目录, 开始编译
cd `pwd`/build &&
    cmake .. &&
    make

# 回到项目根目录
cd ..

# # 把头文件拷贝到 /usr/include/threadpool  so库拷贝到 /usr/lib (系统环境变量 PATH)

# if [ ! -d /usr/include/threadpool ]; then 
#     mkdir /usr/include/threadpool
# fi

# cd `pwd`/include

# # 拷贝 .hpp .h 文件
# for header in `ls *.hpp *.h`
# do
#     cp $header /usr/include/threadpool
# done

# cd ..
# cp `pwd`/lib/libthreadpool.so /usr/lib

# ldconfig