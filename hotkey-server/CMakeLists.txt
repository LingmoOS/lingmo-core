cmake_minimum_required(VERSION 3.15.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Required by pybind11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")

find_package(Python 3 COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

find_package(Qt6 COMPONENTS Core DBus Concurrent REQUIRED)

#遍历所有Cpp文件
file(GLOB_RECURSE CPP_FILES *.cpp *.h)
foreach (filepath ${CPP_FILES})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" filename ${filepath})
    list(APPEND sources_files ${filename})
endforeach (filepath)

add_executable(hotkey-server ${sources_files})

target_include_directories(hotkey-server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(hotkey-server 
    PRIVATE 
    pybind11::embed
    Qt6::Core
    Qt6::DBus
    Qt6::Concurrent
)