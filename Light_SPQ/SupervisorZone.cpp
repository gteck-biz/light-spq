// SupervisorZone.cpp
#include "SupervisorZone.h"

namespace gteck {

    std::mutex printMtx; // Define mutex toàn cục

    SupervisorZone::SupervisorZone(int nodeCount) : running(true) {
        for (int i = 0; i < nodeCount; ++i) {
            nodes.push_back(new ProcessingNode(i));
        }
    }

    SupervisorZone::~SupervisorZone() {
        for (auto node : nodes) {
            delete node;
        }
    }

    void SupervisorZone::pushTask(TaskCallback task) {
        taskQueue.push(task);
        // logSafe("[SZ] Da them mot Task moi vao Queue.");
    }

    void SupervisorZone::runScheduler() {
        logSafe("[SZ] Supervisor bat dau dieu phoi...");

        while (running || !taskQueue.empty()) {
            if (taskQueue.empty()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                continue;
            }

            TaskCallback task = taskQueue.front();
            bool assigned = false;

            for (auto node : nodes) {
                if (node->assignTask(task)) {
                    taskQueue.pop();
                    assigned = true;
                    break;
                }
            }

            if (!assigned) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        logSafe("[SZ] Supervisor dung hoat dong.");
    }

    void SupervisorZone::stop() {
        running = false;
    }

}