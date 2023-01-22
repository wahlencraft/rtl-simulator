#include "thread_pool.h"

#include <iostream>
#include <string>

using namespace std;

string const BOLD = "\033[1m";
string const END = "\033[0m";

atomic_int job_counter = 0;

ThreadPool::ThreadPool(unsigned threads) {
    if (threads == 0 || threads > std::thread::hardware_concurrency())
        threads = std::thread::hardware_concurrency();
    //cout << BOLD << "Using " << threads << " threads" << endl;

    for (unsigned i = 0; i < threads; ++i) {
        std::thread worker([this]() {
        while (true) {
            int job_id;
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
                job_id = job_counter++;
                //std::cout << BOLD << "Popping job #" << job_id << " from the queue\n";
                task = std::move(tasks.front());
                tasks.pop();
                ++running_jobs;
                //std::cout << "There is now " << tasks.size()
                          //<< " jobs left in the queue (+" << running_jobs 
                          //<< " running)" << END << std::endl;
            }
            //std::cout << BOLD << "Start task for job #" << job_id << END << std::endl;
            task();
            --running_jobs;
            //std::cout << BOLD << "Finished job #" << job_id << "\n";
            //std::cout << "There is now " << running_jobs << " running jobs" << END << std::endl;
            cv.notify_all();  // XXX This is not very efficient. Most of the 
                              // time only one thread needs the notify, but
                              // if this was the last job and 
                              // wait_for_jobs_to_finish() is waiting, it is
                              // very important that that thread is notified.
                              // TODO something better.
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
        //std::cout << BOLD << "Enqueueing a job\n";
        if (stop) {
            throw std::runtime_error("Can't enqueue more jobs when the treadpool is stopping.");
        }
        tasks.emplace(job);
        //std::cout << "There is now " << tasks.size()
                  //<< " jobs in the queue (" << running_jobs << " running)" << END << std::endl;
    }
    cv.notify_one();
}

void ThreadPool::wait_for_jobs_to_finish() {
    //cout << BOLD << "Wait for all jobs to finish" << END << endl;
    do {
        std::unique_lock lock(mtx);
        cv.wait(lock, [this]() { return tasks.empty(); });
    } while (running_jobs);
    //cout << BOLD << "All jobs finished" << END << endl;
}

