```markdown
# GTeck Python Bindings (C++17 Backend)

Dự án này cung cấp khả năng điều phối tác vụ đa luồng hiệu năng cao của C++ cho Python thông qua **pybind11**. Hệ thống cho phép bạn thực thi các hàm Python (Callbacks/Lambdas) bên trong các Worker Threads độc lập của C++.

## 📂 Cấu trúc thư mục hiện tại

```text
.
├── Light_SPQ        # C++ Sources
└── Thirty
    └── python
        ├── CMakeLists.txt  # File cấu hình build
        ├── bindings.cpp    # Cầu nối C++/Python
        └── test.py         # Script kiểm tra

```

## 🛠 Yêu cầu hệ thống

* **C++ Compiler:** Hỗ trợ chuẩn C++17 (GCC 7+, Clang 5+, hoặc MSVC 2017+).
* **CMake:** Phiên bản 3.14 trở lên.
* **Python:** 3.6 trở lên.
* **Thư viện:** `pybind11`.
```bash
pip install pybind11

```



## 🏗 Hướng dẫn Build (Biên dịch)

Do file `CMakeLists.txt` nằm trong thư mục `Thirty/python`, quy trình build như sau:

```bash
# 1. Di chuyển vào thư mục chứa cấu hình build
cd Thirty/python

# 2. Tạo thư mục build tạm thời
mkdir build
cd build

# 3. Cấu hình và biên dịch
cmake ..
cmake --build .

```

Sau khi lệnh trên hoàn tất, một file thư viện động (Ví dụ: `gteck_py.cpython-310-x86_64-linux-gnu.so` trên Linux hoặc `gteck_py.pyd` trên Windows) sẽ được tạo ra trong thư mục `build`.

## 🚀 Hướng dẫn sử dụng trong Python

Để sử dụng, bạn chỉ cần import file `.so`/`.pyd` vừa build được.

```python
import sys
import os

# Thêm đường dẫn chứa file build vào Python path
sys.path.append(os.path.abspath("./build"))

import gteck_py
import time

# 1. Khởi tạo Supervisor với 4 Worker Threads (C++ Backend)
sz = gteck_py.SupervisorZone(4)

# 2. Định nghĩa một tác vụ bằng Python
def my_python_task(name):
    print(f"--- Task {name} đang chạy trên C++ Thread ---")
    time.sleep(1)
    print(f"--- Task {name} hoàn tất ---")

# 3. Đẩy tác vụ vào hàng đợi (C++ sẽ điều phối)
sz.pushTask(lambda: my_python_task("Alpha"))
sz.pushTask(lambda: my_python_task("Beta"))

# 4. Chạy bộ lập lịch (Nên chạy trong thread riêng nếu không muốn block main)
import threading
t = threading.Thread(target=sz.runScheduler)
t.start()

# Đợi xử lý...
time.sleep(3)
sz.stop()
t.join()

```

# 📑 Giải thích Cơ chế Vận hành (Under the Hood)

Sự kết hợp giữa **C++17** và **Python** thông qua **pybind11** tạo ra một cơ chế chuyển đổi ngữ cảnh (Context Switching) phức tạp nhưng cực kỳ hiệu quả.

## 1. Luồng dữ liệu và Điều khiển (Workflow)

| Giai đoạn | Hành động | Chi tiết kỹ thuật |
| --- | --- | --- |
| **1. Push Task** | Python gọi `sz.pushTask(py_func)` | `py_func` (một `PyObject`) được pybind11 bọc vào `std::function<void()>`. |
| **2. Queueing** | Lưu trữ vào `std::queue` | Task được lưu trong bộ nhớ heap của C++. Supervisor giữ con trỏ đến hàm Python này. |
| **3. Scheduling** | Supervisor đánh thức Worker | `cv.notify_one()` được gọi. Một luồng C++ (không chịu ảnh hưởng bởi Python GIL) sẽ tỉnh dậy. |
| **4. Execution** | Worker thực thi `task()` | Đây là lúc luồng C++ thâm nhập ngược lại môi trường Python. |

## 2. Cơ chế Quản lý Luồng & GIL (Global Interpreter Lock)

Đây là phần quan trọng nhất để giải thích tại sao hệ thống không bị xung đột bộ nhớ:

### 🔒 GIL Management

Python không cho phép nhiều luồng thực thi bytecode Python cùng lúc.

* Khi luồng C++ của bạn chạy, nó đang nằm ngoài tầm kiểm soát của Python.
* Ngay khi lệnh `task()` (vốn là hàm Python) được gọi trong C++, **pybind11** sẽ thực hiện:
1. **Acquire GIL:** Chiếm quyền điều khiển trình thông dịch Python.
2. **Execute:** Chạy code Python của bạn.
3. **Release GIL:** Trả lại quyền để các luồng Python khác hoặc các Worker khác có thể làm việc.



> **Lưu ý:** Nếu bạn chạy 10 Worker C++ nhưng các hàm bạn đẩy vào toàn là code Python nặng về tính toán (CPU-bound), thì thực tế các Worker sẽ phải xếp hàng chờ GIL, dẫn đến việc xử lý trông có vẻ tuần tự.

---

## 3. Phân tích Hiệu năng (Performance Analysis)

Hệ thống này được tối ưu hóa cho các kịch bản cụ thể:

### ✅ Trường hợp Hiệu quả cao (I/O Bound)

* **Ví dụ:** Gửi API request, Truy vấn Database, Đọc/Ghi file.
* **Lý do:** Khi Python thực hiện I/O, nó tự động **nhả GIL**. Trong lúc Worker 1 đang đợi dữ liệu từ mạng (đã nhả GIL), Worker 2 có thể chiếm GIL để bắt đầu xử lý task tiếp theo.
* **Kết quả:** Tận dụng tối đa đa luồng của C++.

### ⚠️ Trường hợp Nghẽn cổ chai (CPU Bound)

* **Ví dụ:** Xử lý ảnh bằng vòng lặp Python, tính toán số học phức tạp.
* **Lý do:** Luồng C++ chiếm giữ GIL quá lâu để chạy bytecode Python, khiến các Worker khác bị block hoàn toàn.
* **Giải pháp:** Chuyển logic tính toán đó thành một hàm C++ thuần, sau đó chỉ dùng Python để gọi hàm C++ đó.

---

## 4. Bảng so sánh Cơ chế

| Đặc điểm | Luồng thuần Python (`threading`) | Hệ thống GTeck (C++ Worker) |
| --- | --- | --- |
| **Quản lý luồng** | Do Python VM quản lý (hệ điều hành ảo) | Do OS quản lý trực tiếp (Native Thread) |
| **Tải CPU rảnh** | Thường cao do cơ chế polling | Xấp xỉ 0% nhờ `condition_variable` |
| **Độ trễ (Latency)** | Trung bình | Rất thấp (Real-time cấp độ hệ thống) |
| **Khả năng mở rộng** | Bị giới hạn bởi GIL | Mở rộng tốt nếu Task là I/O hoặc C++ thuần |

---

## 💡 Lời khuyên tối ưu

Để đạt hiệu suất "vô địch", bạn nên viết các hàm xử lý dữ liệu thô bằng **C++**, sau đó dùng **Python** để định nghĩa luồng công việc (Workflow). Khi đó, các Worker C++ sẽ chạy song song 100% công suất mà không bao giờ phải đợi GIL của Python.

**Bạn có muốn tôi hướng dẫn cách "nhả GIL" thủ công trong C++ để các Task C++ thuần có thể chạy song song hoàn toàn với Python không?**

## 🤝 Liên hệ

Dự án được phát triển bởi **GTeck Team**.

```

---

### Một vài lưu ý nhỏ khi bạn đưa lên GitHub:
* **.gitignore:** Bạn nên thêm một file `.gitignore` để tránh đẩy các file rác khi build lên. Nội dung file `.gitignore` nên có:
  ```text
  build/
  __pycache__/
  *.so
  *.pyd

```

* **Kiểm tra Include:** Hãy chắc chắn rằng trong file `bindings.cpp`, bạn dùng `#include "SupervisorZone.h"` (không cần thêm đường dẫn dài dòng, vì CMake đã xử lý phần đó cho bạn rồi).

Bạn có cần tôi hỗ trợ viết thêm phần **Unit Test** tự động cho các Node này không?