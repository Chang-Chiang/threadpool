# 指定头文件路径
INCL = -I../src

# 指定 cpp 文件路径
CPP = ../src/*.cpp

# 编译参数 -std=c++17 至少得支持 C++17 标准
CFLAGS = -g 

# 需要编译的文件
all:test

# 文件的编译规则
test:test.cpp  # 所依赖的文件
	g++ $(CFLAGS) -o test test.cpp $(INCL) $(CPP) -lm -lc
	cp test ../bin/.

# 编译的清理
clean:
	rm -f test
