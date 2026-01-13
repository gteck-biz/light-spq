import gteck_py
import time
import threading
import random

def run_system():
    # 1. Khởi tạo Supervisor với 4 Worker C++
    print("[Python] Khoi tao he thong GTeck (C++ Backend)...")
    sz = gteck_py.SupervisorZone(4)

    # 2. Chạy Scheduler trong một luồng Python riêng
    # (Vì hàm runScheduler của C++ là vòng lặp blocking)
    scheduler_thread = threading.Thread(target=sz.runScheduler)
    scheduler_thread.start()

    # 3. Định nghĩa các task Python
    def simple_task():
        # Hàm này sẽ được chạy bởi C++ Worker Thread!
        # Pybind11 tự động lock GIL để chạy code Python này an toàn
        print(f"   (Python Task) Dang chay tren luồng C++ ID: {threading.get_ident()}")
        time.sleep(0.5)

    # 4. Đẩy task vào hệ thống
    print("[Python] Bat dau day task...")

    # Task 1: Hàm bình thường
    sz.pushTask(simple_task)

    # Task 2: Lambda function
    for i in range(5):
        # Lưu ý: capture biến i (default argument hack trong loop)
        sz.pushTask(lambda x=i: print(f"   (Python Lambda) Xu ly du lieu: {x*100}"))
        time.sleep(0.1)

    # Task 3: Task nặng giả lập
    def heavy_task():
        print("   (Python Heavy) Bat dau download file ao...")
        time.sleep(2)
        print("   (Python Heavy) Xong!")

    sz.pushTask(heavy_task)

    # Đợi xíu cho xử lý
    time.sleep(5)

    print("[Python] Yeu cau dung he thong...")
    sz.stop()
    scheduler_thread.join()
    print("[Python] Ket thuc.")

if __name__ == "__main__":
    run_system()