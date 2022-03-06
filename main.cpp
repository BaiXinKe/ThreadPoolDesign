#include <iostream>

#include "ThreadPool.hpp"

void tf(const ThreadSafeQueuePtr<ThreadPool::ThreadTask>& froml,
    const ThreadSafeQueuePtr<ThreadPool::ThreadTask>& fromg)
{
    while (true) {
        ThreadPool::ThreadTask task;
        if (froml->try_pop(task) || fromg->try_pop(task)) {
            task();
        } else {
            std::this_thread::yield();
        }
    }
}

void Hello()
{
    std::cout << "Hello" << std::endl;
}

int main(int, char**)
{
    ThreadPool pool;
    pool.setThreadFunc(tf);
    pool.start();

    pool.submitToNextLocalQueue(Hello);
    pool.submitToGlobalQueue(Hello);
    pool.submitToGlobalQueue(Hello);
    pool.submitToNextLocalQueue(Hello);
}
