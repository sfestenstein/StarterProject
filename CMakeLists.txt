cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-fPIC -Wall -Wextra -Wconversion -Wshadow -Wold-style-cast -Wnon-virtual-dtor)

# Optionally enable ccache if available
option(USE_CCACHE "Enable ccache for faster rebuilds when available" ON)
if(USE_CCACHE)
    find_program(CCACHE_PROGRAM ccache)
    if(CCACHE_PROGRAM)
        message(STATUS "ccache found: ${CCACHE_PROGRAM} — enabling as compiler launcher")
        set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
        set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    else()
        message(STATUS "ccache not found — continuing without compiler launcher")
    endif()
endif()

## Prefer a system or package-provided spdlog if available; fall back to FetchContent
find_package(spdlog QUIET)

if (NOT spdlog_FOUND)
    # Fetch spdlog for fast and easy logging when not already available
    include(FetchContent)
    FetchContent_Declare(
            spdlog
            GIT_REPOSITORY https://github.com/gabime/spdlog.git
            GIT_TAG        v1.15.1
    )

    FetchContent_MakeAvailable(spdlog)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(CommonUtils)
add_subdirectory(Tests)

project(StarterProject CXX)

add_executable(StarterProjectMain src/main.cpp)

target_link_libraries(StarterProjectMain PRIVATE spdlog CommonUtils)

