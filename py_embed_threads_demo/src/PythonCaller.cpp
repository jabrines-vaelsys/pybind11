#include "PythonCaller.h"

#include <iostream>
#include <stdexcept>
#include <utility>

namespace demo {

std::once_flag EmbeddedPython::initFlag;
std::unique_ptr<py::scoped_interpreter> EmbeddedPython::interpreter;
std::unique_ptr<py::gil_scoped_release> EmbeddedPython::releaseMainThreadGil;

void EmbeddedPython::initialize_once(const std::string &moduleDir) {
    std::call_once(initFlag, [&]() {
        interpreter = std::make_unique<py::scoped_interpreter>();
        try {
            py::gil_scoped_acquire gil;
            py::module_ sys = py::module_::import("sys");
            py::list path = sys.attr("path");
            path.append(moduleDir);
        } catch (const py::error_already_set &e) {
            std::cerr << "Failed to initialize Python or set sys.path: " << e.what() << std::endl;
            throw; // unrecoverable at this point
        }
        // Release the main thread GIL; worker threads will acquire as needed
        releaseMainThreadGil = std::make_unique<py::gil_scoped_release>();
    });
}

PythonCaller::PythonCaller(const std::string &instanceName) {
    try {
        py::gil_scoped_acquire gil;
        py::module_ mod = py::module_::import("my_module");
        py::object workerClass = mod.attr("Worker");
        workerInstance = workerClass(instanceName);
    } catch (const py::error_already_set &e) {
        std::cerr << "Error constructing Python Worker instance: " << e.what() << std::endl;
        throw;
    }
}

PythonCaller::~PythonCaller() {
    try {
        py::gil_scoped_acquire gil;
        // Explicitly reset to drop Python references while holding the GIL
        workerInstance = py::none();
    } catch (...) {
        // Suppress all exceptions in destructor
    }
}

std::string PythonCaller::greet(const std::string &who) {
    try {
        py::gil_scoped_acquire gil;
        py::object result = workerInstance.attr("greet")(who);
        return result.cast<std::string>();
    } catch (const py::error_already_set &e) {
        std::cerr << "Python exception in greet: " << e.what() << std::endl;
        throw;
    }
}

int PythonCaller::compute_sum(int a, int b) {
    try {
        py::gil_scoped_acquire gil;
        py::object result = workerInstance.attr("compute")(a, b);
        return result.cast<int>();
    } catch (const py::error_already_set &e) {
        std::cerr << "Python exception in compute_sum: " << e.what() << std::endl;
        throw;
    }
}

int PythonCaller::might_fail(int x) {
    try {
        py::gil_scoped_acquire gil;
        py::object result = workerInstance.attr("might_fail")(x);
        return result.cast<int>();
    } catch (const py::error_already_set &e) {
        std::cerr << "Python exception in might_fail: " << e.what() << std::endl;
        // Re-throw to let caller decide how to handle failure
        throw;
    }
}

} // namespace demo