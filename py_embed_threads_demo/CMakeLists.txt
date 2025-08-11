cmake_minimum_required(VERSION 3.16)

project(PyEmbedThreadsDemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find system Python (headers + library) for embedding
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Absolute path to our embedded Python module directory
set(PY_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/python)

add_executable(py_embed_threads
    src/main.cpp
    src/PythonCaller.cpp
)

# Include pybind11 headers from the checked-out repo
# The headers are located one level up under include/
set(PYBIND11_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)

target_include_directories(py_embed_threads PRIVATE src ${PYBIND11_INCLUDE_DIR})

target_link_libraries(py_embed_threads PRIVATE Python3::Python)

# Safer Python C API behavior
target_compile_definitions(py_embed_threads PRIVATE PY_SSIZE_T_CLEAN)

# Provide the module directory at compile time as a string literal
# Ensure it is seen by the compiler as a C++ string literal
# Example expansion: -DPY_MODULE_DIR="/abs/path/python"
target_compile_definitions(py_embed_threads PRIVATE PY_MODULE_DIR=\"${PY_MODULE_DIR}\")

# On Unix-like systems, prefer full RPATH for convenience when running from the build tree
set_target_properties(py_embed_threads PROPERTIES
    BUILD_RPATH_USE_ORIGIN ON
)