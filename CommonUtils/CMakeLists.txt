cmake_minimum_required(VERSION 3.20)
project(CommonUtils CXX)

FetchContent_MakeAvailable(spdlog)

file(GLOB CommonUtil_source     "*.cpp")
file(GLOB CommonUtil_headers     "*.h")


add_library(CommonUtils SHARED ${CommonUtil_source} ${CommonUtil_headers})
target_link_libraries(CommonUtils PRIVATE spdlog)

