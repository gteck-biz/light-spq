
# GTeck Async Task Scheduler

Hệ thống xử lý tác vụ bất đồng bộ đa luồng (Multi-threaded Asynchronous Task Scheduler) được viết bằng C++ (Standard 11+). Hệ thống triển khai mô hình **Producer-Consumer** kết hợp với mẫu thiết kế **Active Object**, cho phép quản lý và thực thi các tác vụ song song một cách hiệu quả thông qua cơ chế Thread Pool.

![C++](https://img.shields.io/badge/Language-C++11-blue.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)
![Namespace](https://img.shields.io/badge/Namespace-gteck-orange.svg)

## 📖 Mục lục

-   [Giới thiệu](#giới-thiệu)
-   [Tính năng nổi bật](#tính-năng-nổi-bật)
-   [Kiến trúc hệ thống](#kiến-trúc-hệ-thống)
-   [Cấu trúc thư mục](#cấu-trúc-thư-mục)
-   [Cài đặt và Biên dịch](#cài-đặt-và-biên-dịch)
-   [Hướng dẫn sử dụng](#hướng-dẫn-sử-dụng)
-   [Tác giả](#tác-giả)

---

## 🚀 Giới thiệu

**GTeck Task Scheduler** giải quyết vấn đề tắc nghẽn (blocking) trong các ứng dụng đơn luồng bằng cách phân phối tác vụ cho các **Processing Nodes (Worker Threads)** chạy ngầm.

Hệ thống được thiết kế để:
1.  **Tách biệt** luồng chính (Main Thread) khỏi việc xử lý nặng.
2.  **Tái sử dụng** luồng (Thread Reusability) để tránh chi phí khởi tạo liên tục.
3.  **Linh hoạt** trong loại tác vụ xử lý thông qua cơ chế Callback (`std::function`).

## ✨ Tính năng nổi bật

* **Namespace `gteck`:** Code được đóng gói sạch sẽ, tránh xung đột tên.
* **Generic Tasks:** Hỗ trợ xử lý mọi loại tác vụ (Function Pointer, Lambda Expression) thay vì chỉ xử lý dữ liệu cố định.
* **Smart Waiting:** Sử dụng `std::condition_variable` giúp Worker Nodes ngủ hoàn toàn (0% CPU usage) khi không có việc.
* **Thread Safety:** Đảm bảo an toàn dữ liệu với `std::mutex` và `std::atomic`.
* **Active Object Pattern:** Mỗi Node là một thực thể độc lập, tự quản lý vòng đời và trạng thái của nó.

## 🏗 Kiến trúc hệ thống

Hệ thống bao gồm 2 thành phần chính:

1.  **Supervisor Zone (SZ):**
    * Quản lý hàng đợi tác vụ (`Task Queue`).
    * Điều phối (Scheduler) tác vụ đến các Node đang rảnh.
    * Cơ chế: First-Come-First-Served.

2.  **Processing Node (PN):**
    * Chạy trên một luồng riêng biệt (`std::thread`).
    * Hoạt động theo cơ chế: `Wait` -> `Wake up` -> `Process` -> `Wait`.

## 📂 Cấu trúc thư mục

```text
.
├── Common.h             # Thư viện chung và định nghĩa kiểu dữ liệu
├── ProcessingNode.h     # Khai báo lớp Node xử lý (Worker)
├── ProcessingNode.cpp   # Hiện thực logic của Node
├── SupervisorZone.h     # Khai báo lớp Quản lý (Scheduler)
├── SupervisorZone.cpp   # Hiện thực logic điều phối
├── main.cpp             # Chương trình chính (Demo)
└── README.md            # Tài liệu hướng dẫn

```

## ⚙️ Cài đặt và Biên dịch

Yêu cầu: Trình biên dịch hỗ trợ **C++11** trở lên (GCC, Clang, MSVC).

### Trên Linux / MacOS (sử dụng g++)

Sử dụng Terminal, di chuyển đến thư mục dự án và chạy lệnh:

```bash
# Biên dịch tất cả các file .cpp
g++ -o gteck_scheduler main.cpp SupervisorZone.cpp ProcessingNode.cpp -pthread

# Chạy chương trình
./gteck_scheduler

```

*Lưu ý: Cờ `-pthread` là bắt buộc trên Linux để sử dụng thư viện luồng.*

### Trên Windows (sử dụng MinGW hoặc Visual Studio)

**MinGW:**

```bash
g++ -o gteck_scheduler.exe main.cpp SupervisorZone.cpp ProcessingNode.cpp
gteck_scheduler.exe

```

**Visual Studio:**

1. Tạo một `Empty Project`.
2. Add tất cả các file `.h` và `.cpp` vào project.
3. Nhấn `F5` để Build và Run.

## 💡 Hướng dẫn sử dụng

### 1. Khởi tạo hệ thống

```cpp
#include "SupervisorZone.h"

// Khởi tạo Supervisor với 4 Worker Nodes
gteck::SupervisorZone sz(4);

// Chạy bộ điều phối ở một luồng riêng
std::thread schedulerThread(&gteck::SupervisorZone::runScheduler, &sz);

```

### 2. Thêm tác vụ (Push Task)

Bạn có thể đẩy bất kỳ hàm `void()` nào vào hệ thống.

**Cách 1: Sử dụng Lambda Function (Khuyên dùng)**

```cpp
// Tính toán nặng
sz.pushTask([]() {
    int ketqua = 0;
    for(int i = 0; i < 1000000; i++) ketqua += i;
    // Lưu ý: Sử dụng gteck::logSafe để in ra màn hình an toàn
});

// Capture biến bên ngoài
int ID = 101;
sz.pushTask([ID]() {
    gteck::logSafe("Đang xử lý ID: " + std::to_string(ID));
});

```

**Cách 2: Sử dụng Function Pointer**

```cpp
void hamXuLy() {
    // Làm gì đó...
}

sz.pushTask(hamXuLy);

```

### 3. Dừng hệ thống

```cpp
// Dừng nhận task và đợi xử lý hết hàng đợi
sz.stop();

// Join luồng điều phối
if (schedulerThread.joinable()) {
    schedulerThread.join();
}

```

## 🤝 Đóng góp (Contributing)

Mọi đóng góp đều được hoan nghênh! Vui lòng tạo **Pull Request** hoặc mở **Issue** nếu bạn tìm thấy lỗi.

1. Fork dự án.
2. Tạo nhánh tính năng (`git checkout -b feature/NewFeature`).
3. Commit thay đổi (`git commit -m 'Add some NewFeature'`).
4. Push lên nhánh (`git push origin feature/NewFeature`).
5. Tạo Pull Request.

## 📄 License

Dự án này được cấp phép theo giấy phép [MIT License](https://www.google.com/search?q=LICENSE).

---

**Developed by GTeck Team.**

```

```