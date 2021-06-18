# 简介

yvent是仿照muduo实现的一个基于Reactor模式的多线程C++网络库，简化部分如下：

* 多线程依赖于C++11提供的std::thread库，而不是重新封装POSIX thread API。 
* 定时器依赖于C++11提供的std::chrono库，而不是封装timestamp类，不用关心单位
* 仅具有简单的日志输出功能，用于调试。
* 仅使用epoll，不使用poll和select。

# 实例

yvent/example下实现了一个简单的web服务器


# 编译：

```shell
$ cd yvent
$ ./build.sh 
```

