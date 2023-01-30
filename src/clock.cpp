#include <iostream>

#include "clock.h"

using namespace std;

Clock::Clock(unsigned max_threads):
    clockables{},
    thread_count{(max_threads == 0) ? thread::hardware_concurrency() : min(max_threads, thread::hardware_concurrency())},
    start_barrier{thread_count + 1},
    set_chain_barrier{thread_count},
    reset_chain_barrier{thread_count},
    done_barrier{thread_count + 1} {

    for (unsigned i=0; i<thread_count; ++i) {
        threads.push_back(thread([this, i](){process(i);}));
    }
}

Clock::Clock(std::initializer_list<Clockable*> clockables):
    clockables{clockables},
    thread_count{thread::hardware_concurrency()},
    start_barrier{thread_count + 1},
    set_chain_barrier{thread_count},
    reset_chain_barrier{thread_count},
    done_barrier{thread_count + 1} {

    for (unsigned i=0; i<thread_count; ++i) {
        threads.push_back(thread([this, i](){process(i);}));
    }
}

Clock::Clock(unsigned max_threads, std::initializer_list<Clockable*> clockables):
    clockables{clockables},
    thread_count{(max_threads == 0) ? thread::hardware_concurrency() : min(max_threads, thread::hardware_concurrency())},
    start_barrier{thread_count + 1},
    set_chain_barrier{thread_count},
    reset_chain_barrier{thread_count},
    done_barrier{thread_count + 1} {

    for (unsigned i=0; i<thread_count; ++i) {
        threads.emplace_back(thread([this, i](){process(i);}));
    }
}

Clock::~Clock() {
    running = false;
    start_barrier.arrive();
    for (unsigned i=0; i<thread_count; ++i) {
        threads[i].join();
    }
}

void Clock::add_clockable(Clockable *clockable) {
    clockables.push_back(clockable);
}

void Clock::process(int thread_number) {
    while (true) {

        // Wait for next clock cycle
        //cout << "T" << thread_number << " wait for next clock()" << endl;
        start_barrier.arrive_and_wait();
        if (!running) {
            break;
        }

        //cout << "T" << thread_number << " start set-chains" << endl;

        // Set chain
        for (size_t i = 0 + thread_number; i < clockables.size(); i += thread_count) {
            //cout << "T" << thread_number << ": clockables[" << i << "]" << endl;
            clockables[i]->start_set_chain();
        }
        set_chain_barrier.arrive_and_wait();

        // Rest chain
        for (size_t i = 0 + thread_number; i < clockables.size(); i += thread_count) {
            clockables[i]->start_reset_chain();
        }

        reset_chain_barrier.arrive_and_wait();

        // Clock
        for (size_t i = 0 + thread_number; i < clockables.size(); i += thread_count) {
            clockables[i]->clock();
        }
        done_barrier.arrive();
    }
}

void Clock::clock() {

    // Start all threads
    start_barrier.arrive();

    // Wait for all threads to be done
    done_barrier.arrive_and_wait();
}
