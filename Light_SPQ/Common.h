// Common.h
#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <functional> // Thư viện quan trọng cho callback

namespace gteck {
    extern std::mutex printMtx;

    // Định nghĩa kiểu dữ liệu Callback: Một hàm void không tham số
    using TaskCallback = std::function<void()>;

    inline void logSafe(const std::string& msg) {
        std::lock_guard<std::mutex> lock(printMtx);
        std::cout << msg << std::endl;
    }
}

#endif