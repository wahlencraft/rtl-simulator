#ifndef CLOCK_H_
#define CLOCK_H_

#include <list>

#include "clockable.h"

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
    unsigned const max_threads = 0;

};

#endif  // CLOCK_H_

