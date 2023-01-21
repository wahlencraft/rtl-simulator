#include <iostream>

#include "clock.h"
#include "thread_pool.h"

using namespace std;

Clock::Clock(): clockables{} {}
Clock::Clock(std::initializer_list<Clockable*> clockables): clockables{clockables} {}

void Clock::add_clockable(Clockable *clockable) {
    clockables.push_back(clockable);
}

void Clock::clock() {

    cout << "\nStarting clockcycle " << cycle << endl;
    ++cycle;

    ThreadPool thread_pool{4};

    for (Clockable *clockable : clockables) {
        thread_pool.enqueue( [clockable]() { clockable->start_set_chain(); } );
    }

    cout << "\nSet chain done" << endl;

    thread_pool.wait_for_empty_queue();

    for (Clockable *clockable : clockables) {
        thread_pool.enqueue( [clockable]() { clockable->start_reset_chain(); } );
    }

    thread_pool.wait_for_empty_queue();

    for (Clockable *clockable : clockables) {
        // Clock is very fast and does not start a chain. It is probably faster
        // to do all of them in one thread.
        clockable->clock();
    }
}
