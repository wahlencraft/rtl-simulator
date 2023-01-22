#ifndef CLOCK_H_
#define CLOCK_H_

#include <list>

#include "clockable.h"
#include "thread_pool.h"

class Clock {
public:
    Clock(unsigned max_threads=0);
    Clock(std::initializer_list<Clockable*> clockables);
    Clock(unsigned max_threads, std::initializer_list<Clockable*> clockables);

    void add_clockable(Clockable *clockable);
    void clock();

private:
    long long unsigned cycle{0};
    std::list<Clockable*> clockables;
    ThreadPool thread_pool;

};

#endif  // CLOCK_H_

