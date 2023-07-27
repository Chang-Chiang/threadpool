cmake_minimum_required(VERSION 3.0)
project(threadpool)

# 设置so库生成路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 设置调试信息
# C++14 make_ptr
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17 -fPIC")

# 加载子目录
add_subdirectory(src)