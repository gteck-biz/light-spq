cmake_minimum_required(VERSION 3.14)
project(GTeckSystem)

# ==========================================
# 1. CẤU HÌNH C++ STANDARD 17
# ==========================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Tắt các extension riêng của compiler để đảm bảo chuẩn

# ==========================================
# 2. TÌM THƯ VIỆN THREADS (ĐA LUỒNG)
# ==========================================
# CMake sẽ tự động thêm -pthread trên Linux/Mac
find_package(Threads REQUIRED)

message(STATUS "The value of MY_VARIABLE is: ${SOURCES}")
# ==========================================
# 3. TARGET: CHẠY C++ STANDALONE
# (Dành cho việc chạy ./gteck_run)
# ==========================================
# Liệt kê các file nguồn (TRỪ bindings.cpp)
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../Light_SPQ")
set(SOURCE_FILES
    ${SRC_DIR}/../main.cpp
    ${SRC_DIR}/SupervisorZone.cpp
    ${SRC_DIR}/ProcessingNode.cpp
)

#file(GLOB SOURCE_FILES "${SRC_DIR}/Light_SPQ/*.cpp")

add_executable(gteck_run ${SOURCE_FILES})

# Link thư viện threads vào file thực thi
#target_include_directories(gteck_run PRIVATE ${SRC_DIR})

target_include_directories(gteck_run PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}        # Thư mục hiện tại
        ${SRC_DIR}                     # Thư mục gốc chứa các file .h
)
target_link_libraries(gteck_run PRIVATE Threads::Threads)

# ==========================================
# 4. TARGET: PYTHON BINDINGS (OPTIONAL)
# (Dành cho import gteck_py trong Python)
# ==========================================
# Phần này sẽ kiểm tra xem bạn có cài pybind11 chưa
find_package(pybind11 CONFIG QUIET)

if(pybind11_FOUND)
    message(STATUS "Found pybind11 v${pybind11_VERSION}: Building Python extension.")
    
    # Tạo module Python (không bao gồm main.cpp)
    pybind11_add_module(gteck_py 
        bindings.cpp
        ${SRC_DIR}/SupervisorZone.cpp
        ${SRC_DIR}/ProcessingNode.cpp
    )
    
    # Link threads cho module Python
    target_include_directories(gteck_py PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}        # Thư mục hiện tại
            ${SRC_DIR}                     # Thư mục gốc chứa các file .h
    )
    target_link_libraries(gteck_py PRIVATE Threads::Threads)
else()
    message(WARNING "pybind11 not found. Skipping Python bindings build.")
    message(STATUS "Hint: Install via 'pip install pybind11[global]' or 'apt install pybind11-dev'")
endif()