project(LoggingTests)

add_executable(LoggingConfigTest LoggerConfigUt.cpp)
add_executable(LogUtilsTest LogUtilsUt.cpp)
add_executable(SinkFactoryTest SinkFactoryUt.cpp)

target_link_libraries(LoggingConfigTest gtest_main Logging)
target_link_libraries(LogUtilsTest gtest_main Logging)
target_link_libraries(SinkFactoryTest gtest_main Logging)

enable_testing()

add_test(NAME LoggingConfigTest COMMAND LoggingConfigTest)
add_test(NAME LogUtilsTest COMMAND LogUtilsTest)
add_test(NAME SinkFactoryTest COMMAND SinkFactoryTest)