project(CommonUtilsTests)


# Add the source files
add_executable(TimerTest TimerUt.cpp ${CMAKE_SOURCE_DIR}/CommonUtils/Timer.cpp)
add_executable(DataHandlerTest DataHandlerUt.cpp )

# Include directories
include_directories(${CMAKE_SOURCE_DIR})

# Link Google Test libraries
target_link_libraries(TimerTest gtest_main)
target_link_libraries(DataHandlerTest gtest_main)

# Enable testing
enable_testing()

# Add tests
add_test(NAME TimerTest COMMAND TimerTest)
add_test(NAME DataHandlerTest COMMAND DataHandlerTest)
