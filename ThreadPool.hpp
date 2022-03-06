#ifndef THREAD_POOL_HPP__
#define THREAD_POOL_HPP__

#include <thread>
#include <vector>

#include <functional>
#include <future>
#include <type_traits>

#include "ThreadSafeQueue.hpp"

class ThreadPool {
public:
    using ThreadTask = std::function<void()>;
    using ThreadFunc = std::function<void(const ThreadSafeQueuePtr<ThreadTask>&,
        const ThreadSafeQueuePtr<ThreadTask>&)>;

    static constexpr size_t DEFAULT_THREAD_NUM { 0 };

    explicit ThreadPool(size_t numThread = DEFAULT_THREAD_NUM);
    void setThreadFunc(ThreadFunc func)
    {
        threadFunc_ = func;
        funcSetted_ = true;
    }

    void start();

    template <typename Func, typename... Args>
    auto submitToNextLocalQueue(Func&& func, Args&&... args)
        -> std::future<std::invoke_result_t<Func, Args...>>;

    template <typename Func, typename... Args>
    auto submitToGlobalQueue(Func&& func, Args&&... args)
        -> std::future<std::invoke_result_t<Func, Args...>>;

    ~ThreadPool();

private:
    bool funcSetted_;
    std::atomic<bool> stop_;

    std::promise<void> start_;
    std::future<void> start_fut_;

    std::atomic<uint64_t> locQueueIndex_;
    ThreadFunc threadFunc_;
    std::vector<std::thread> thread_;
    std::size_t numThread_;

    std::vector<ThreadSafeQueuePtr<ThreadTask>> lQueues_;
    ThreadSafeQueuePtr<ThreadTask> gQueue_;
};

template <typename Func, typename... Args>
auto ThreadPool::submitToNextLocalQueue(Func&& func, Args&&... args)
    -> std::future<std::invoke_result_t<Func, Args...>>
{
    using result_t = std::invoke_result_t<Func, Args...>;

    auto taskPackage = std::make_shared<std::packaged_task<result_t()>>(
        std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
    std::future<result_t> ret = taskPackage->get_future();

    uint64_t cur = locQueueIndex_.fetch_add(1) % lQueues_.size();
    lQueues_[cur]->push([taskPackage] { (*taskPackage)(); });

    return ret;
}

template <typename Func, typename... Args>
auto ThreadPool::submitToGlobalQueue(Func&& func, Args&&... args)
    -> std::future<std::invoke_result_t<Func, Args...>>
{
    using result_t = std::invoke_result_t<Func, Args...>;

    auto taskPackage = std::make_shared<std::packaged_task<result_t()>>(
        std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
    std::future<result_t> ret = taskPackage->get_future();
    gQueue_->push([taskPackage] { (*taskPackage)(); });

    return ret;
}

#endif