import sys
import re
import statistics

def parse_and_aggregate(input_text):
    # 存储数据的字典: { "Test Name": [value1, value2, ...] }
    data = {}
    
    # 正则表达式用于清理测试名称中的干扰项
    # 匹配 :wavy_dash: 或者 (Unstable ...) 及其后面的内容
    cleanup_pattern = re.compile(r'(:wavy_dash:|\(Unstable.*?\))')

    lines = input_text.strip().split('\n')
    
    for line in lines:
        line = line.strip()
        # 跳过非数据行（nanobench 的每一行数据都以 | 开头）
        if not line.startswith('|'):
            continue
        
        parts = [p.strip() for p in line.split('|')]
        
        # 确保列数足够 (nanobench 默认格式通常有 6-7 列)
        # 索引: 0=空, 1=百分比, 2=主要数值, 3=次要数值, ... 最后一列=名字
        if len(parts) < 6:
            continue

        try:
            # 第 2 列是主要衡量指标 (27,692,019.00)，去掉逗号转 float
            value_str = parts[2].replace(',', '')
            value = float(value_str)
            
            # 最后一列是名字
            raw_name = parts[-1]
            
            # 清洗名字：去掉 unstable 警告和图标
            clean_name = cleanup_pattern.sub('', raw_name).strip()
            # 去掉名字前后的反引号 `
            clean_name = clean_name.replace('`', '')
            
            if clean_name not in data:
                data[clean_name] = []
            data[clean_name].append(value)
            
        except ValueError:
            # 这一行可能不是数据行，或者是表头
            continue

    return data

def print_summary(data):
    if not data:
        print("未找到有效的 benchmark 数据行。")
        return

    print("-" * 95)
    # 打印表头
    print(f"{'Benchmark Name':<35} | {'Samples':<7} | {'Avg Value':>15} | {'Min Value':>15} | {'Max Value':>15}")
    print("-" * 95)

    # 按平均值从小到大排序 (假设数值越小越好，如耗时/周期)
    # 如果数值越大越好 (如 ops/s)，可以去掉 reverse=False
    sorted_items = sorted(data.items(), key=lambda item: statistics.mean(item[1]), reverse=False)

    first_avg = None

    for name, values in sorted_items:
        avg_val = statistics.mean(values)
        min_val = min(values)
        max_val = max(values)
        count = len(values)
        
        if first_avg is None:
            first_avg = avg_val
            diff_str = "(100.0%)"
        else:
            # 计算相对于第一名的差距
            percent = (first_avg / avg_val) * 100
            diff_str = f"({percent:.1f}%)"

        print(f"{name:<35} | {count:<7} | {avg_val:,.2f} {diff_str:>8} | {min_val:,.2f} | {max_val:,.2f}")

    print("-" * 95)
    print("注意: 括号内的百分比表示相对于第一名的性能比率 (越高越好，基准为 100%)")

# --- 主程序入口 ---

# 你可以将你的日志直接粘贴在这个变量里测试
sample_input = """
|    100.0% |       27,692,019.00 |               36.11 |    4.9% |      0.30 | `ks lockfree queue bench test`
|     89.9% |       30,801,805.00 |               32.47 |   12.4% |      0.31 | :wavy_dash: `boost lockfree queue bench test` (Unstable with ~1.0 iters. Increase `minEpochIterations` to e.g. 10)
|     93.6% |       29,586,775.00 |               33.80 |    3.0% |      0.32 | `ks lockfree queue bench test`
|     77.5% |       35,728,570.00 |               27.99 |    4.2% |      0.38 | `boost lockfree queue bench test`
|     73.3% |       37,780,913.00 |               26.47 |    6.3% |      0.41 | :wavy_dash: `ks lockfree queue bench test` (Unstable with ~1.0 iters. Increase `minEpochIterations` to e.g. 10)
|     73.9% |       37,463,205.00 |               26.69 |    4.2% |      0.41 | `boost lockfree queue bench test`
|     73.4% |       37,717,946.00 |               26.51 |    2.0% |      0.42 | `ks lockfree queue bench test`
|     66.8% |       41,433,465.00 |               24.14 |    5.9% |      0.46 | :wavy_dash: `boost lockfree queue bench test` (Unstable with ~1.0 iters. Increase `minEpochIterations` to e.g. 10)
|     68.2% |       40,600,869.00 |               24.63 |    1.0% |      0.44 | `ks lockfree queue bench test`
|     66.8% |       41,462,665.00 |               24.12 |    4.5% |      0.45 | `boost lockfree queue bench test`
|     70.0% |       39,558,687.00 |               25.28 |    2.7% |      0.43 | `ks lockfree queue bench test`
|     67.7% |       40,930,424.00 |               24.43 |    3.6% |      0.46 | `boost lockfree queue bench test`
|     67.6% |       40,945,043.00 |               24.42 |    1.9% |      0.44 | `ks lockfree queue bench test`
|     63.9% |       43,347,156.00 |               23.07 |    7.4% |      0.47 | :wavy_dash: `boost lockfree queue bench test` (Unstable with ~1.0 iters. Increase `minEpochIterations` to e.g. 10)
|     68.5% |       40,428,653.00 |               24.73 |    4.9% |      0.44 | `ks lockfree queue bench test`
|     60.8% |       45,578,064.00 |               21.94 |    0.8% |      0.49 | `boost lockfree queue bench test`
|     68.4% |       40,502,035.00 |               24.69 |    5.0% |      0.45 | :wavy_dash: `ks lockfree queue bench test` (Unstable with ~1.0 iters. Increase `minEpochIterations` to e.g. 10)
|     63.1% |       43,914,807.00 |               22.77 |    2.7% |      0.48 | `boost lockfree queue bench test`
|     66.6% |       41,566,949.00 |               24.06 |    1.5% |      0.45 | `ks lockfree queue bench test`
|     60.3% |       45,935,495.00 |               21.77 |    3.5% |      0.50 | `boost lockfree queue bench test`
|     66.4% |       41,729,512.00 |               23.96 |    4.0% |      0.44 | `ks lockfree queue bench test`
"""

# 如果有命令行输入，优先读取文件或 stdin；否则使用上面的 sample_input
if len(sys.argv) > 1 or not sys.stdin.isatty():
    # 从 stdin 读取 (例如: cat bench_result.txt | python analyze.py)
    input_content = sys.stdin.read()
else:
    input_content = sample_input

parsed_data = parse_and_aggregate(input_content)
print_summary(parsed_data)