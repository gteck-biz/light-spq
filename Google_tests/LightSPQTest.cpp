#include "gtest/gtest.h"
#include "SupervisorZone.h"
#include  "Common.h"
#include <random>

using namespace gteck;

class LightSPQFixture : public ::testing::Test {
    protected:
        virtual void SetUp()
        {
        }

        virtual void TearDown() {
            // delete pnode;
        }
};


// Một hàm bình thường để test callback
void regularFunction() {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    logSafe("      (Task A) Day la ham thong thuong (Function Pointer).");
}

TEST_F(LightSPQFixture, NewProcessingNode){ // 1/1/1 -> 1/3/1
    // rand(time(0));

    SupervisorZone sz(4); // 4 Worker
    std::thread schedulerThread(&SupervisorZone::runScheduler, &sz);

    // --- VÍ DỤ 1: Đẩy một hàm thường vào ---
    sz.pushTask(regularFunction);

    // --- VÍ DỤ 2: Đẩy Lambda function (Xử lý tác vụ tính toán) ---
    for (int i = 1; i <= 5; ++i) {
        // [=] capture i by value để dùng trong lambda
        sz.pushTask([i]() {
            int processingTime = 500 + (rand() % 1000);
            std::this_thread::sleep_for(std::chrono::milliseconds(processingTime));

            int result = i * i;
            // Lưu ý: Việc in ấn này thực hiện bên trong Node Worker
            logSafe("      (Task B) Tinh binh phuong cua " + std::to_string(i)
                    + " = " + std::to_string(result));
        });
    }

    // --- VÍ DỤ 3: Đẩy Lambda function (Xử lý giả lập download file nặng) ---
    std::vector<std::string> files = {"image.png", "video.mp4", "data.json"};
    for (const auto& file : files) {
        sz.pushTask([file]() {
            logSafe("      (Task C) Dang tai xuong file: " + file);
            std::this_thread::sleep_for(std::chrono::seconds(2)); // Giả lập tốn thời gian
            logSafe("      (Task C) -> Tai xong: " + file);
        });
    }

    // Đợi một chút để xem kết quả
    std::this_thread::sleep_for(std::chrono::seconds(8));

    sz.stop();
    if (schedulerThread.joinable()) schedulerThread.join();

    std::cout << "HE THONG GTECK KET THUC." << std::endl;

    EXPECT_EQ(1,1);
}
