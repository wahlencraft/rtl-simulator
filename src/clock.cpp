#include <iostream>

#include "clock.h"

using namespace std;

Clock::Clock(): clockables{} {}
Clock::Clock(std::initializer_list<Clockable*> clockables): clockables{clockables} {}

void Clock::add_clockable(Clockable *clockable) {
    clockables.push_back(clockable);
}

void Clock::clock() {

    cout << "\nStarting clockcycle " << cycle << endl;
    ++cycle;

    for (Clockable *clockable : clockables) {
        // TODO spawn threads
        clockable->start_set_chain();
    }
    // TODO join threads
    for (Clockable *clockable : clockables) {
        // TODO spawn threads
        clockable->start_reset_chain();
    }
    // TODO join threads

    for (Clockable *clockable : clockables) {
        clockable->clock();
    }
}
