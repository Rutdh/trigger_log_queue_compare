# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目概述

这是一个 C++17 无锁队列性能对比项目，用于评估不同队列实现在高并发风控日志处理场景下的性能表现。

## 构建与测试

### 构建命令

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### 运行测试

```bash
# 在 build 目录下
./trigger_log_queue_compare_benchmark
```

### 运行单元测试

```bash
cd build
ctest
```

## 架构概览

### 核心队列实现（include/）

1. **lockfree_queue.h** - 自定义无锁队列，使用信号量进行阻塞同步
2. **lockfree_queue_no_block.h** - 非阻塞版本的自定义无锁队列
3. **boost::lockfree::queue** - Boost 库的标准实现（作为对比基准）

### 主要组件

- **CRiskTriggerLogData** - 风控触发日志数据封装
- **SystemContext** - 全局系统上下文，管理 gflags 和 glog 初始化
- **RiskTriggerLogThread** - 风控日志处理线程类
- **EpochBarrier** (include/EpochBarrier.h) - 屏障同步机制（当前为空实现）

### 性能测试框架

使用 `ankerl::nanobench` 进行基准测试，采用多线程生产者-消费者模式：
- 队列容量：4096
- 生产者线程：3 个
- 消费者线程：1 个
- 每个生产者产生 4096 条记录
- 总计 30 轮随机顺序测试（避免缓存偏差）

## 关键依赖

- **C++17** - 编译标准
- **CMake 3.19+** - 构建系统
- **GTest/GMock** - 单元测试框架
- **nanobench** - 性能基准测试
- **glog/gflags** - 日志和命令行参数
- **Protobuf** - 数据序列化
- **Boost lockfree** - 对比用队列实现
- **Abseil** - 高级 C++ 库

## 代码约定

1. 使用智能指针 (`std::unique_ptr`) 管理数据生命周期
2. 使用移动语义避免不必要的拷贝
3. 错误处理使用返回值而非异常
4. 线程安全通过原子操作保证
5. 日志统一使用 glog，支持颜色输出和磁盘写入
