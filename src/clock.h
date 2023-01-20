#ifndef CLOCK_H_
#define CLOCK_H_

#include <list>

#include "clockable.h"

class Clock {
public:
    Clock();
    Clock(std::initializer_list<Clockable*> clockables);

    void add_clockable(Clockable *clockable);
    void clock();

private:
    long long unsigned cycle{0};
    std::list<Clockable*> clockables;

};

#endif  // CLOCK_H_

