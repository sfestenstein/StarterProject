cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-fPIC -Wall -Wextra)

#Fetch spdlog for fast and easy logging!
include(FetchContent)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.12.0
)
FetchContent_MakeAvailable(spdlog)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(CommonUtils)

project(StarterProject CXX)

add_executable(StarterProjectMain src/main.cpp)

target_link_libraries(StarterProjectMain PRIVATE CommonUtils spdlog)

