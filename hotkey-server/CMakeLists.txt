cmake_minimum_required(VERSION 3.15.0)
project(TestPybind VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package (Python 3 COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

add_executable(TestPybind main.cpp)

target_link_libraries(TestPybind PRIVATE pybind11::embed)

