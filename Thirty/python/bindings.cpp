// bindings.cpp
#include <pybind11/pybind11.h>
#include <pybind11/functional.h> // QUAN TRỌNG: Để hỗ trợ callback
#include <pybind11/stl.h>        // Để hỗ trợ string, vector...

#include "SupervisorZone.h"

namespace py = pybind11;

// Định nghĩa module tên là "gteck_py"
PYBIND11_MODULE(gteck_py, m) {
    m.doc() = "GTeck Async Task Scheduler Python Bindings"; // Optional module docstring

    // Binding cho class SupervisorZone
    py::class_<gteck::SupervisorZone>(m, "SupervisorZone")
        .def(py::init<int>(), py::arg("nodeCount")) // Constructor
        
        // Binding cho hàm pushTask
        // pybind11 tự động chuyển Python function -> std::function<void()>
        .def("pushTask", &gteck::SupervisorZone::pushTask)
        
        // Binding cho hàm runScheduler
        // call_guard<py::gil_scoped_release>() giúp nhả GIL khi chạy hàm này 
        // để không chặn các luồng Python khác (nếu chạy blocking)
        .def("runScheduler", &gteck::SupervisorZone::runScheduler, 
             py::call_guard<py::gil_scoped_release>())
        
        .def("stop", &gteck::SupervisorZone::stop);
}