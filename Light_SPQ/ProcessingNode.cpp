// ProcessingNode.cpp
#include "ProcessingNode.h"

namespace gteck {

    ProcessingNode::ProcessingNode(int nodeId)
        : id(nodeId), hasTask(false), stopRequest(false) {
        workerThread = std::thread(&ProcessingNode::run, this);
    }

    ProcessingNode::~ProcessingNode() {
        stop();
    }

    void ProcessingNode::run() {
        while (true) {
            std::unique_lock<std::mutex> lock(nodeMtx);

            // Chờ đợi task
            cv.wait(lock, [this] { return hasTask || stopRequest; });

            if (stopRequest && !hasTask) break;

            // Lấy task ra để thực thi
            TaskCallback taskToRun = currentTask;

            // Mở khóa để chạy task (không chặn các luồng khác)
            lock.unlock();

            try {
                // --- THỰC THI CALLBACK TẠI ĐÂY ---
                // Node không cần biết task làm gì, chỉ cần gọi nó
                if (taskToRun) {
                    logSafe("   [PN-" + std::to_string(id) + "] -> Bat dau thuc thi Callback.");
                    taskToRun(); // Gọi hàm callback
                }
            } catch (...) {
                logSafe("   [PN-" + std::to_string(id) + "] -> Loi khi thuc thi Callback!");
            }

            logSafe("   [PN-" + std::to_string(id) + "] -> Hoan thanh task.");

            // Reset trạng thái
            lock.lock();
            hasTask = false;
        }
    }

    bool ProcessingNode::assignTask(TaskCallback task) {
        std::lock_guard<std::mutex> lock(nodeMtx);
        if (hasTask) {
            return false;
        }

        currentTask = task; // Gán hàm callback
        hasTask = true;
        cv.notify_one();
        return true;
    }

    bool ProcessingNode::isBusy() {
        std::lock_guard<std::mutex> lock(nodeMtx);
        return hasTask;
    }

    void ProcessingNode::stop() {
        {
            std::lock_guard<std::mutex> lock(nodeMtx);
            stopRequest = true;
        }
        cv.notify_one();
        if (workerThread.joinable()) {
            workerThread.join();
        }
    }

}