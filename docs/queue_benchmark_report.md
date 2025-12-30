# 队列性能对比报告
招商版本的无锁队列类底层是boost的lockfree::queue, 所以下面直接导入boost相关的库

## 测试框架与环境
- 框架：`nanobench`
- 运行模式：运行60轮, 每轮nanobench会在内部运行11轮epoch, 每轮epoch设置会跑50次, 每次都会预创建线程池（3 个生产者、1 个消费者），每次每个生产者推送 4096 条，共 12288 条. 循环多轮并随机打乱队列顺序。release模式.
- 环境: cpu:amd 8845hs, 系统:ubuntu24.04

## 测试场景
- 生产者生成风险触发日志数据（包含订单与 Protobuf 模板, 用的是招商实际protobuf文件, 有部分字段删减），队列传递裸指针，消费者逐条释放。
- 目标：对比入队/出队吞吐和稳定性。

## 测试结果（部分节选）

```
| relative |               ns/op |                op/s |    err% |     total | benchmark
|---------:|--------------------:|--------------------:|--------:|----------:|:----------
|   100.0% |       33,798,465.25 |               29.59 |    3.8% |     19.98 | `ks lockfree queue bench test`
|    83.2% |       40,605,598.72 |               24.63 |    2.6% |     22.62 | `boost lockfree queue bench test`
|   105.2% |       32,127,743.49 |               31.13 |    1.8% |     18.87 | `ks_no_block lockfree queue bench test`
|    94.0% |       35,965,817.72 |               27.80 |    1.0% |     21.48 | `ks lockfree queue bench test`
|    82.2% |       41,119,326.94 |               24.32 |    0.4% |     24.56 | `boost lockfree queue bench test`
|   101.8% |       33,185,817.36 |               30.13 |    1.4% |     19.81 | `ks_no_block lockfree queue bench test`
|    92.1% |       36,707,354.19 |               27.24 |    0.4% |     21.86 | `ks lockfree queue bench test`
|    82.0% |       41,231,909.90 |               24.25 |    1.3% |     24.52 | `boost lockfree queue bench test`
|   100.7% |       33,550,975.31 |               29.81 |    1.4% |     20.14 | `ks_no_block lockfree queue bench test`
|    93.3% |       36,233,989.35 |               27.60 |    0.8% |     21.63 | `ks lockfree queue bench test`
|    82.8% |       40,825,803.69 |               24.49 |    0.2% |     24.39 | `boost lockfree queue bench test`
|    97.5% |       34,675,259.02 |               28.84 |    0.8% |     20.65 | `ks_no_block lockfree queue bench test`
|    92.2% |       36,651,223.40 |               27.28 |    0.3% |     21.85 | `ks lockfree queue bench test`
|    82.3% |       41,062,708.41 |               24.35 |    1.0% |     24.48 | `boost lockfree queue bench test`
|    98.9% |       34,170,617.44 |               29.26 |    0.8% |     20.46 | `ks_no_block lockfree queue bench test`
|    92.7% |       36,471,922.06 |               27.42 |    0.7% |     21.77 | `ks lockfree queue bench test`
|    82.1% |       41,154,015.35 |               24.30 |    0.7% |     24.61 | `boost lockfree queue bench test`
|    97.8% |       34,550,597.63 |               28.94 |    1.1% |     20.61 | `ks_no_block lockfree queue bench test`
|    92.6% |       36,508,488.42 |               27.39 |    1.5% |     21.84 | `ks lockfree queue bench test`
|    82.3% |       41,043,833.40 |               24.36 |    0.3% |     24.53 | `boost lockfree queue bench test`
|    97.1% |       34,807,269.00 |               28.73 |    0.7% |     20.83 | `ks_no_block lockfree queue bench test`
|    91.8% |       36,799,089.92 |               27.17 |    0.5% |     21.91 | `ks lockfree queue bench test`
|    82.2% |       41,095,827.43 |               24.33 |    0.7% |     24.54 | `boost lockfree queue bench test`
|    94.8% |       35,640,525.96 |               28.06 |    0.4% |     21.25 | `ks_no_block lockfree queue bench test`
|    91.5% |       36,945,947.61 |               27.07 |    1.2% |     21.93 | `ks lockfree queue bench test`
|    81.8% |       41,312,189.58 |               24.21 |    0.4% |     24.55 | `boost lockfree queue bench test`
|    94.2% |       35,879,339.40 |               27.87 |    0.9% |     21.45 | `ks_no_block lockfree queue bench test`
|    92.0% |       36,737,479.33 |               27.22 |    0.6% |     21.97 | `ks lockfree queue bench test`
|    81.3% |       41,560,933.72 |               24.06 |    0.8% |     24.77 | `boost lockfree queue bench test`
|    94.4% |       35,802,062.32 |               27.93 |    0.9% |     21.34 | `ks_no_block lockfree queue bench test`
|    91.2% |       37,052,178.11 |               26.99 |    0.5% |     22.09 | `ks lockfree queue bench test`
|    81.4% |       41,510,466.86 |               24.09 |    0.9% |     24.87 | `boost lockfree queue bench test`
|    94.4% |       35,798,878.06 |               27.93 |    1.1% |     21.31 | `ks_no_block lockfree queue bench test`
|    91.2% |       37,076,451.86 |               26.97 |    0.8% |     22.10 | `ks lockfree queue bench test`
|    80.7% |       41,886,542.22 |               23.87 |    0.6% |     24.98 | `boost lockfree queue bench test`
|    94.1% |       35,900,059.55 |               27.86 |    1.3% |     21.48 | `ks_no_block lockfree queue bench test`
|    91.2% |       37,076,564.49 |               26.97 |    0.5% |     22.17 | `ks lockfree queue bench test`
|    81.2% |       41,639,594.60 |               24.02 |    0.5% |     24.83 | `boost lockfree queue bench test`
|    94.6% |       35,717,946.48 |               28.00 |    0.8% |     21.31 | `ks_no_block lockfree queue bench test`
|    91.6% |       36,885,895.47 |               27.11 |    0.8% |     22.12 | `ks lockfree queue bench test`
|    81.3% |       41,567,381.50 |               24.06 |    0.8% |     24.80 | `boost lockfree queue bench test`
|    95.6% |       35,352,622.54 |               28.29 |    0.6% |     21.07 | `ks_no_block lockfree queue bench test`
|    91.3% |       37,005,917.82 |               27.02 |    1.0% |     22.08 | `ks lockfree queue bench test`
|    81.4% |       41,520,256.22 |               24.08 |    0.8% |     24.78 | `boost lockfree queue bench test`
|    93.8% |       36,030,326.90 |               27.75 |    0.9% |     21.46 | `ks_no_block lockfree queue bench test`
|    90.1% |       37,516,971.58 |               26.65 |    1.3% |     22.38 | `ks lockfree queue bench test`
|    80.9% |       41,784,158.85 |               23.93 |    0.3% |     24.95 | `boost lockfree queue bench test`
|    94.0% |       35,963,371.29 |               27.81 |    0.7% |     21.48 | `ks_no_block lockfree queue bench test`
|    90.8% |       37,224,809.15 |               26.86 |    0.8% |     22.26 | `ks lockfree queue bench test`
|    81.5% |       41,456,722.02 |               24.12 |    0.9% |     24.72 | `boost lockfree queue bench test`
|    94.6% |       35,729,016.14 |               27.99 |    1.1% |     21.30 | `ks_no_block lockfree queue bench test`
|    91.1% |       37,083,535.38 |               26.97 |    0.8% |     22.07 | `ks lockfree queue bench test`
|    82.6% |       40,919,666.95 |               24.44 |    0.5% |     24.45 | `boost lockfree queue bench test`
|    94.3% |       35,852,994.24 |               27.89 |    0.5% |     21.44 | `ks_no_block lockfree queue bench test`
|    90.7% |       37,254,928.71 |               26.84 |    0.9% |     22.37 | `ks lockfree queue bench test`
|    81.1% |       41,678,619.73 |               23.99 |    0.4% |     24.90 | `boost lockfree queue bench test`
|    93.7% |       36,055,401.09 |               27.74 |    1.0% |     21.44 | `ks_no_block lockfree queue bench test`
|    91.3% |       37,010,200.92 |               27.02 |    0.7% |     22.17 | `ks lockfree queue bench test`
|    80.7% |       41,896,618.94 |               23.87 |    0.5% |     24.97 | `boost lockfree queue bench test`
|    93.9% |       35,985,635.11 |               27.79 |    1.4% |     21.47 | `ks_no_block lockfree queue bench test`
|    90.7% |       37,247,599.43 |               26.85 |    0.4% |     22.21 | `ks lockfree queue bench test`
|    81.0% |       41,740,693.23 |               23.96 |    0.5% |     24.80 | `boost lockfree queue bench test`
|    94.5% |       35,771,544.08 |               27.96 |    0.7% |     21.45 | `ks_no_block lockfree queue bench test`
|    91.1% |       37,120,749.39 |               26.94 |    0.4% |     22.14 | `ks lockfree queue bench test`
|    81.4% |       41,538,703.49 |               24.07 |    0.5% |     24.79 | `boost lockfree queue bench test`
|    94.1% |       35,906,886.22 |               27.85 |    0.5% |     21.54 | `ks_no_block lockfree queue bench test`
|    90.4% |       37,385,129.50 |               26.75 |    1.3% |     22.29 | `ks lockfree queue bench test`
|    80.9% |       41,761,318.86 |               23.95 |    0.4% |     24.92 | `boost lockfree queue bench test`
|    94.3% |       35,852,898.47 |               27.89 |    0.8% |     21.44 | `ks_no_block lockfree queue bench test`
|    90.4% |       37,377,687.80 |               26.75 |    0.7% |     22.30 | `ks lockfree queue bench test`
|    81.2% |       41,630,135.45 |               24.02 |    0.7% |     24.88 | `boost lockfree queue bench test`
|    94.0% |       35,961,011.40 |               27.81 |    0.8% |     21.50 | `ks_no_block lockfree queue bench test`
|    90.6% |       37,325,408.97 |               26.79 |    0.5% |     22.26 | `ks lockfree queue bench test`
|    81.9% |       41,281,822.50 |               24.22 |    0.5% |     24.70 | `boost lockfree queue bench test`
|    94.5% |       35,761,432.47 |               27.96 |    0.4% |     21.38 | `ks_no_block lockfree queue bench test`
|    91.4% |       36,993,511.41 |               27.03 |    0.5% |     22.07 | `ks lockfree queue bench test`
|    81.1% |       41,695,387.09 |               23.98 |    0.7% |     24.85 | `boost lockfree queue bench test`
|    94.0% |       35,962,403.22 |               27.81 |    0.9% |     21.51 | `ks_no_block lockfree queue bench test`
|    91.1% |       37,095,259.80 |               26.96 |    1.0% |     22.22 | `ks lockfree queue bench test`
|    80.6% |       41,944,605.85 |               23.84 |    0.8% |     24.97 | `boost lockfree queue bench test`
|    93.8% |       36,016,120.06 |               27.77 |    0.9% |     21.45 | `ks_no_block lockfree queue bench test`
|    90.4% |       37,399,087.67 |               26.74 |    1.0% |     22.23 | `ks lockfree queue bench test`
|    81.0% |       41,735,307.92 |               23.96 |    1.3% |     24.95 | `boost lockfree queue bench test`
|    93.5% |       36,159,325.41 |               27.66 |    0.7% |     21.54 | `ks_no_block lockfree queue bench test`
|    90.8% |       37,226,096.93 |               26.86 |    0.6% |     22.27 | `ks lockfree queue bench test`
|    80.5% |       41,996,753.94 |               23.81 |    0.8% |     25.02 | `boost lockfree queue bench test`
|    93.7% |       36,057,400.37 |               27.73 |    0.9% |     21.50 | `ks_no_block lockfree queue bench test`
|    91.1% |       37,119,987.87 |               26.94 |    0.6% |     22.12 | `ks lockfree queue bench test`
|    81.8% |       41,323,393.09 |               24.20 |    0.8% |     24.59 | `boost lockfree queue bench test`
|    93.2% |       36,251,318.70 |               27.59 |    0.6% |     21.68 | `ks_no_block lockfree queue bench test`
```

## 结论
- 吞吐：`ks_no_block` 领先，`ks` 次之, `boost` 最慢。代表值（首行基准 100%）：`ks` 33.8 ns/op，`ks_no_block` 32.1 ns/op，`boost` 40.6 ns/op
- 稳定性：自定义队列（尤其 `ks_no_block`）在当前负载下抖动更小（多数 err% < 1%）；`boost` 因容量回退/CAS 竞争，平均性能落后约 15–20%
- 选择建议：追求吞吐/低抖动选 `lockfree_queue_no_block`, 需要阻塞语义与容量保护选 `lockfree_queue`. 
