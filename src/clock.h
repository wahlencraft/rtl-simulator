#ifndef CLOCK_H_
#define CLOCK_H_

#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>

#include "clockable.h"

class Clock {
public:
    Clock(unsigned max_threads=0);
    Clock(std::initializer_list<Clockable*> clockables);
    Clock(unsigned max_threads, std::initializer_list<Clockable*> clockables);
    ~Clock();

    void add_clockable(Clockable *clockable);
    void clock();

private:
    void process(int thread_number);

    long long unsigned cycle{0};
    std::vector<Clockable*> clockables;
    unsigned thread_count;
    std::mutex mtx{};
    std::condition_variable cv{};
    std::vector<std::thread> threads{};

    // Only modify these when mtx locked
    bool start = false;
    bool running = true;
    unsigned waiting_for_start_count = 0;
    unsigned set_chain_count = 0;
    unsigned reset_chain_count = 0;
    unsigned clock_count = 0;
    unsigned done_count = 0;

};

#endif  // CLOCK_H_

