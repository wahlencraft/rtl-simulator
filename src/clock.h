#ifndef CLOCK_H_
#define CLOCK_H_

#include <vector>
#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <atomic>

#include "clockable.h"
#include "barrier.h"

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
    unsigned const thread_count;
    std::vector<std::thread> threads{};

    std::atomic_bool running = true;

    Barrier start_barrier;
    Barrier set_chain_barrier;
    Barrier reset_chain_barrier;
    Barrier done_barrier;

};

#endif  // CLOCK_H_

