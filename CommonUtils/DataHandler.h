#include <functional>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <thread>
#include <atomic>
namespace CommonUtils
{
/**
 * @class DataHandler
 * @brief Implements a thread safe queue.
 *        * Listeners can register an std::function to listen for new data
 *        * Any thread can signal that new data is available.
 *        * listener functions are executed in a common, but separate thread,
 */
template <typename T>
class DataHandler {
public:
    using Listener = std::function<void(const T&)>;

    /**
     * @brief Constructor
     */
    DataHandler() :
        stopFlag_(false)
    {
        workerThread_ = std::thread(&DataHandler::processData, this);
    }

    /**
     * Cleanly destroys an object of this type!
     */
    ~DataHandler()
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stopFlag_ = true;
        }
        condVar_.notify_all();
        if (workerThread_.joinable())
        {
            workerThread_.join();
        }
        listeners_.clear();
        while (!dataQueue_.empty())
        {
            dataQueue_.pop();
        }
    }

    /**
     * @brief Indicates new data is available for the listeners to consume.
     * @param data
     */
    void signalData(const T& data)
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            dataQueue_.push(data);
        }
        condVar_.notify_one();
    }

    /**
     * @brief Allows anyone to register a function to be called
     *        when new data is signaled.  Listener functions are
     *        passed by weak pointer to allow for easy 'unregistration'
     * @param listener
     */
    void registerListener(std::weak_ptr<Listener> listener)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        listeners_.push_back(listener);
    }

    /**
     * @brief Returns usage statistics of this class
     * @return std pair, first item is the number of listeners,
     *                   second is the number of dataum in the
     *                   queue.
     */
    std::pair <size_t, size_t> watermarkInfo()
    {
        return {listeners_.size(), dataQueue_.size()};
    }

private:
    void processData() {
        while (true) {
            T data;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                condVar_.wait(lock, [this] { return !dataQueue_.empty() || stopFlag_; });
                if (stopFlag_ && dataQueue_.empty()) {
                    return;
                }
                data = dataQueue_.front();
                dataQueue_.pop();
            }
            notifyListeners(data);
        }
    }

    void notifyListeners(const T& data) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto it = listeners_.begin(); it != listeners_.end(); ) {
            if (auto listener = it->lock()) {
                (*listener)(data);
                ++it;
            } else {
                it = listeners_.erase(it);
            }
        }
    }

    std::vector<std::weak_ptr<Listener>> listeners_;
    std::queue<T> dataQueue_;
    std::mutex mutex_;
    std::condition_variable condVar_;
    std::thread workerThread_;
    std::atomic<bool> stopFlag_;
};
}
