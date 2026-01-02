// ProcessingNode.h
#ifndef PROCESSING_NODE_H
#define PROCESSING_NODE_H

#include "Common.h"

namespace gteck {

    class ProcessingNode {
    private:
        int id;
        std::thread workerThread;
        std::mutex nodeMtx;
        std::condition_variable cv;

        TaskCallback currentTask; // Lưu trữ hàm callback
        bool hasTask;
        bool stopRequest;

        void run();

    public:
        ProcessingNode(int nodeId);
        ~ProcessingNode();

        // Nhận vào một hàm callback thay vì string
        bool assignTask(TaskCallback task);

        bool isBusy();
        void stop();
    };

}

#endif