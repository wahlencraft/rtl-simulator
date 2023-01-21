#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#include <mutex>
#include <thread>
#include <condition_variable>
#include <atomic>

#include <vector>
#include <queue>
#include <functional>

class ThreadPool {
public:
    ThreadPool(ThreadPool const &) = delete;
    ThreadPool(ThreadPool &&) = delete;
    ThreadPool &operator=(ThreadPool const &) = delete;
    ThreadPool &operator=(ThreadPool &&) = delete;

    ThreadPool(unsigned threads=0);
    ~ThreadPool();

    void enqueue(std::function<void()> job);
    void wait_for_jobs_to_finish();

private:
    bool stop = false;
    std::atomic_int running_jobs{0};
    std::vector<std::thread> workers{};
    std::queue<std::function<void()>> tasks{};
    std::mutex mtx{};
    std::condition_variable cv{};
};

#endif  // THREAD_POOL_H_

