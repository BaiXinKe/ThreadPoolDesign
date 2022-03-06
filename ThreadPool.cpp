#include "ThreadPool.hpp"
#include <cassert>

ThreadPool::ThreadPool(size_t numThread)
    : numThread_ { numThread == DEFAULT_THREAD_NUM ? std::thread::hardware_concurrency() : numThread }
    , locQueueIndex_ { 0 }
    , funcSetted_ { false }
    , gQueue_ { std::make_unique<ThreadSafeQueue<ThreadTask>>() }
{
    assert(numThread_ > 0);
    for (int i = 0; i < numThread_; i++) {
        lQueues_.emplace_back(new ThreadSafeQueue<ThreadTask>);
    }
}

void ThreadPool::start()
{
    assert(funcSetted_);
    for (int i = 0; i < numThread_; i++) {
        ThreadSafeQueuePtr<ThreadTask>& currThreadLocal = lQueues_[i];
        thread_.emplace_back(threadFunc_, std::cref(currThreadLocal), std::cref(gQueue_));
    }
}

ThreadPool::~ThreadPool()
{
    for (auto& thread : thread_) {
        if (thread.joinable())
            thread.join();
    }
}