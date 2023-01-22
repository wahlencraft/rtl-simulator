#include <iostream>

#include "clock.h"

using namespace std;

Clock::Clock(unsigned max_threads): clockables{}, thread_pool{max_threads} {}
Clock::Clock(std::initializer_list<Clockable*> clockables):
    clockables{clockables}, thread_pool{0} {}
Clock::Clock(unsigned max_threads, std::initializer_list<Clockable*> clockables):
    clockables{clockables}, thread_pool{max_threads} {}

void Clock::add_clockable(Clockable *clockable) {
    clockables.push_back(clockable);
}

void Clock::clock() {

    //cout << "\nStarting clockcycle " << cycle << endl;
    ++cycle;

    for (Clockable *clockable : clockables) {
        thread_pool.enqueue( [clockable]() { clockable->start_set_chain(); } );
    }

    //cout << "\nSet chain started\n" << endl;

    thread_pool.wait_for_jobs_to_finish();

    //cout << "\nSet chain done\n" << endl;

    for (Clockable *clockable : clockables) {
        thread_pool.enqueue( [clockable]() { clockable->start_reset_chain(); } );
    }

    thread_pool.wait_for_jobs_to_finish();

    for (Clockable *clockable : clockables) {
        // Clock is very fast and does not start a chain. It is probably faster
        // to do all of them in one thread.
        clockable->clock();
    }
}
