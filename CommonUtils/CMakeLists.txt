cmake_minimum_required(VERSION 3.20)
project(CommonUtils CXX)

FetchContent_MakeAvailable(spdlog)

add_library(CommonUtils SHARED GeneralLogger.cpp)
target_link_libraries(CommonUtils PRIVATE spdlog)

