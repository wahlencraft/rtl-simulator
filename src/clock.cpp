#include <iostream>

#include "clock.h"

using namespace std;

Clock::Clock(unsigned max_threads):
    clockables{},
    thread_count{min(max_threads, thread::hardware_concurrency())} {
    if (thread_count == 0)
        thread_count = thread::hardware_concurrency();

    for (unsigned i=0; i<thread_count; ++i) {
        threads.push_back(thread([this, i](){process(i);}));
    }
}

Clock::Clock(std::initializer_list<Clockable*> clockables):
    clockables{clockables},
    thread_count{thread::hardware_concurrency()} {

    for (unsigned i=0; i<thread_count; ++i) {
        threads.push_back(thread([this, i](){process(i);}));
    }
}

Clock::Clock(unsigned max_threads, std::initializer_list<Clockable*> clockables):
    clockables{clockables},
    thread_count{min(max_threads, thread::hardware_concurrency())} {
    if (thread_count == 0)
        thread_count = thread::hardware_concurrency();

    //cout << "Spawning " << max_threads << " threads" << endl;
    for (unsigned i=0; i<thread_count; ++i) {
        threads.emplace_back(thread([this, i](){process(i);}));
    }
}

Clock::~Clock() {
    //cout << "~Clock()" << endl;
    {
        std::unique_lock lock(mtx);
        running = false;
        //cout << "~Clock()" << endl;
    }
    cv.notify_all();
    for (unsigned i=0; i<thread_count; ++i) {
        threads[i].join();
    }
    //cout << "~Clock()" << endl;
}

void Clock::add_clockable(Clockable *clockable) {
    clockables.push_back(clockable);
}

void Clock::process(int thread_number) {
    while (true) {

        // Wait untill start == true
        // and ++waiting_for_start_count = 0
        bool running_copy;
        {
            std::unique_lock lock(mtx);
            waiting_for_start_count = (waiting_for_start_count + 1) % thread_count;
            //cout << "T" << thread_number << " waiting for start:" 
                 //<< start << " wait_count=" << waiting_for_start_count << endl;
            cv.wait(lock, [this]() { return (start || (!running)) && (waiting_for_start_count == 0); });
            running_copy = running;
        }
        cv.notify_all();
        if (!running_copy) {
            //cout << "Not running, abort" << endl;
            break;
        }

        //cout << "Start set-chains " << thread_number << endl;

        // Set chain
        for (size_t i = 0 + thread_number; i < clockables.size(); i += thread_count) {
            //cout << "S c[" << i << "]" << endl;
            clockables[i]->start_set_chain();
        }
        {
            std::unique_lock lock(mtx);
            set_chain_count = (set_chain_count + 1) % thread_count;
            //cout << "Start chains " << thread_number << " done (" << set_chain_count << ")" << endl;
            cv.wait(lock, [this]() { return set_chain_count == 0; });
        }
        cv.notify_all();

        //cout << "Start reset-chains " << thread_number << endl;
        // Rest chain
        for (size_t i = 0 + thread_number; i < clockables.size(); i += thread_count) {
            //cout << "R c[" << i << "]" << endl;
            clockables[i]->start_reset_chain();
        }
        {
            std::unique_lock lock(mtx);
            reset_chain_count = (reset_chain_count + 1) % thread_count;
            //cout << "Reset-chains " << thread_number << " done (" << reset_chain_count << ")" << endl;
            start = false;
            cv.wait(lock, [this]() { return reset_chain_count == 0; });
        }
        cv.notify_all();

        // Clock
        for (size_t i = 0 + thread_number; i < clockables.size(); i += thread_count) {
            //cout << "C c[" << i << "]" << endl;
            clockables[i]->clock();
        }
        {
            std::unique_lock lock(mtx);
            done_count = (done_count + 1) % (thread_count + 1);
            //cout << "Clocks " << thread_number << " done (" << done_count << ")" << endl;
            //cv.wait(lock, [this]() { return done_count == 0; });
        }
        cv.notify_all();
    }
}

void Clock::clock() {
    //cout << "\nNew clock cycle" << endl;

    // Start all threads
    {
        std::unique_lock lock(mtx);
        start = true;
    }
    cv.notify_all();

    // Wait for all threads to be done
    {
        std::unique_lock lock(mtx);
        //cout << "Wait for all threads to be done" << endl;
        done_count = (done_count + 1) % (thread_count + 1);
        cv.wait(lock, [this]() { return done_count == 0; });
    }
    //cout << "All threads done" << endl;
    cv.notify_all();

}
