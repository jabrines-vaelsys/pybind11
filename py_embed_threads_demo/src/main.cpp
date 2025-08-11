#include "PythonCaller.h"

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#ifndef PY_MODULE_DIR
#error "PY_MODULE_DIR not defined"
#endif

using namespace demo;

int main() {
    try {
        EmbeddedPython::initialize_once(PY_MODULE_DIR);
    } catch (const std::exception &e) {
        std::cerr << "Failed to initialize embedded Python: " << e.what() << std::endl;
        return 1;
    }

    const int numThreads = 4;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([i]() {
            try {
                PythonCaller caller("worker_" + std::to_string(i));

                auto greetMsg = caller.greet("C++ thread " + std::to_string(i));
                std::cout << "[Thread " << i << "] greet => " << greetMsg << std::endl;

                int s = caller.compute_sum(i, i * 2);
                std::cout << "[Thread " << i << "] compute_sum => " << s << std::endl;

                try {
                    int v = caller.might_fail(i);
                    std::cout << "[Thread " << i << "] might_fail => " << v << std::endl;
                } catch (const py::error_already_set &e) {
                    std::cerr << "[Thread " << i << "] Python raised as expected in might_fail: " << e.what() << std::endl;
                }
            } catch (const std::exception &e) {
                std::cerr << "[Thread " << i << "] Unhandled exception: " << e.what() << std::endl;
            }
        });
    }

    for (auto &t : threads) t.join();

    std::cout << "All threads finished." << std::endl;
    return 0;
}