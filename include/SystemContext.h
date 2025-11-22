#pragma once

#include <string>
#include <iostream>
#include <filesystem>
#include <gflags/gflags.h>
#include <glog/logging.h>

class SystemContext {
public:
    // 禁止拷贝和赋值，确保全局只有一个 Context 管理器
    SystemContext(const SystemContext&) = delete;
    SystemContext& operator=(const SystemContext&) = delete;

    /**
     * @brief 构造函数：初始化 Gflags 和 Glog
     * @param argc 指针
     * @param argv 指针
     * @param app_name 程序名称（用于日志前缀）
     * @param log_dir 日志存放目录（默认 ./logs）
     */
    SystemContext(int* argc, char*** argv, const std::string& app_name, const std::string& log_dir = "./logs") {
        // 1. 初始化 Gflags
        gflags::ParseCommandLineFlags(argc, argv, true);

        // 2. 准备日志目录 (使用 C++17 filesystem)
        if (!std::filesystem::exists(log_dir)) {
            std::filesystem::create_directories(log_dir);
            std::cout << "[System] Created log directory: " << log_dir << std::endl;
        }

        // 3. 配置 Glog 选项
        FLAGS_alsologtostderr = true;           // 同时输出到控制台
        FLAGS_colorlogtostderr = true;          // 彩色输出
        FLAGS_log_dir = log_dir;                // 设置日志目录
        FLAGS_minloglevel = google::INFO;       // 最小日志级别
        FLAGS_stop_logging_if_full_disk = true; // 磁盘满时停止日志

        // 4. 初始化 Glog
        google::InitGoogleLogging(app_name.c_str());

        // 5. 安装信号处理 (Crash 时打印堆栈)
        google::InstallFailureSignalHandler();
        
        LOG(INFO) << "=== System Initialized: " << app_name << " ===";
    }

    // 析构函数：负责清理和刷盘
    ~SystemContext() {
        LOG(INFO) << "=== System Shutdown ===";
        
        // 关闭 Glog (这会强制 flush 所有日志)
        google::ShutdownGoogleLogging();
        
        // 清理 Gflags
        gflags::ShutDownCommandLineFlags();
    }
};