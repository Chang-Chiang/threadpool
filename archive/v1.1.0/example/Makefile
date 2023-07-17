testserver :
    # -l 指定库名, -L 指定库路径 -Wl,-rpath='../lib' 指定运行时动态库搜索路径
	g++ -o testTreadPool test.cpp -L ../lib -lthreadpool -lpthread -g -Wl,-rpath='../lib'

clean :
	rm -f testTreadPool
