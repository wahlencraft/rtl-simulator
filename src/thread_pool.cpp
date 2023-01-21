#include "thread_pool.h"

#include <iostream>
#include <string>

using namespace std;

string const BOLD = "\033[1m";
string const END = "\033[0m";

ThreadPool::ThreadPool(int threads): threads{threads} {
    for (int i = 0; i < threads; ++i) {
        std::thread worker([this]() {
        while (true) {
            std::function<void()> task;
            /* pop a task from queue, and execute it. */
            {
                std::unique_lock lock(mtx);
                cv.wait(lock, [this]() { return stop || !tasks.empty(); });
                if (stop && tasks.empty())
                    return;
                /* even if stop = 1, once tasks is not empty, then
                 * excucte the task until tasks queue become empty
                 */
                std::cout << BOLD << "Popping job from the queue\n";
                task = std::move(tasks.front());
                tasks.pop();
                std::cout << "There is now " << tasks.size()
                          << " jobs left in the queue" << END << std::endl;
            }
            std::cout << BOLD << "Start task for job" << END << std::endl;
            task();
            std::cout << BOLD << "Finished job" << END << std::endl;
            cv.notify_one();
            }
        });
        workers.emplace_back(std::move(worker));
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(mtx);
        stop = true;
    }
    cv.notify_all();
    for (auto &worker : workers) {
        worker.join();
    }
}

void ThreadPool::enqueue(std::function<void()> job) {
    {
        std::unique_lock<std::mutex> lock(mtx);
        std::cout << BOLD << "Enqueueing a job\n";
        if (stop) {
            throw std::runtime_error("Can't enqueue more jobs when the treadpool is stopping.");
        }
        tasks.emplace(job);
        std::cout << "There is now " << tasks.size()
                  << " jobs in the queue" << END << std::endl;
    }
    cv.notify_one();
}

void ThreadPool::wait_for_empty_queue() {
    std::unique_lock lock(mtx);
    cv.wait(lock, [this]() { return tasks.empty(); });
}

