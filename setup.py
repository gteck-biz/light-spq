# setup.py
from setuptools import setup, Extension
import pybind11

# Định nghĩa các file nguồn C++ cần biên dịch
cpp_args = ['-std=c++17', '-O3'] # Cờ biên dịch tối ưu hóa

ext_modules = [
    Extension(
        'gteck_py',  # Tên module sẽ import trong Python
        sources=[
            'bindings.cpp',
            'Light_SPQ/ProcessingNode.cpp',
            'Light_SPQ/SupervisorZone.cpp'
        ],
        include_dirs=[
            pybind11.get_include(), # Đường dẫn đến header pybind11
            '.'                     # Đường dẫn hiện tại để tìm header .h
        ],
        language='c++',
        extra_compile_args=cpp_args,
    ),
]

setup(
    name='gteck_py',
    version='1.0',
    description='Python bindings for GTeck Task Scheduler',
    ext_modules=ext_modules,
)