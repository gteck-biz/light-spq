// SupervisorZone.h
#ifndef SUPERVISOR_ZONE_H
#define SUPERVISOR_ZONE_H

#include "Common.h"
#include "ProcessingNode.h"

namespace gteck {

    class SupervisorZone {
    private:
        std::vector<ProcessingNode*> nodes;
        std::queue<TaskCallback> taskQueue; // Hàng đợi chứa các hàm
        bool running;

    public:
        SupervisorZone(int nodeCount);
        ~SupervisorZone();

        // Push một hàm vào hàng đợi
        void pushTask(TaskCallback task);

        void runScheduler();
        void stop();
    };

}

#endif