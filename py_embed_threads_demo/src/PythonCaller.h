#pragma once

#include <pybind11/embed.h>
#include <memory>
#include <mutex>
#include <string>

namespace demo {

namespace py = pybind11;

class EmbeddedPython {
public:
    // Initialize the embedded interpreter once, append moduleDir to sys.path.
    static void initialize_once(const std::string &moduleDir);

private:
    static std::once_flag initFlag;
    static std::unique_ptr<py::scoped_interpreter> interpreter;
    // Keep the main thread's GIL released for the duration of the program
    static std::unique_ptr<py::gil_scoped_release> releaseMainThreadGil;
};

class PythonCaller {
public:
    explicit PythonCaller(const std::string &instanceName);
    ~PythonCaller();

    std::string greet(const std::string &who);
    int compute_sum(int a, int b);
    int might_fail(int x);

private:
    py::object workerInstance; // Instance of Python class
};

} // namespace demo