#include "CommonUtils/DataHandler.h"
#include <gtest/gtest.h>
#include <memory>
#include <chrono>
#include <thread>

TEST(DataHandlerTest, SignalDataNotifiesListeners) 
{
    CommonUtils::DataHandler<int> handler;

    auto listener1 = [](const int& data) {
        EXPECT_EQ(data, 42);
    };

    auto listener2 = [](const int& data) {
        EXPECT_EQ(data, 42);
    };

    handler.registerListener(listener1);
    handler.registerListener(listener2);

    handler.signalData(42);

    // Give some time for the worker thread to process the data
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(DataHandlerTest, ExpiredListenersAreRemoved) 
{
    CommonUtils::DataHandler<int> handler;

    auto listener1 = [](const int& data) {
        EXPECT_EQ(data, 42);
    };

    auto regId1 = handler.registerListener(listener1);

    auto listener2 = [](const int& data) {
        EXPECT_EQ(data, 42);
    };

    auto regId2 = handler.registerListener(listener2);

    handler.signalData(42);

    // Give some time for the worker thread to process the data
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Only one listener should remain
    EXPECT_EQ(handler.watermarkInfo().first, 2);

    handler.unregisterListener(regId1);

    // Only one listener should remain
    EXPECT_EQ(handler.watermarkInfo().first, 1);

    handler.unregisterListener(regId2);

    // Only one listener should remain
    EXPECT_EQ(handler.watermarkInfo().first, 0);

}

int main(int argc, char **argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}