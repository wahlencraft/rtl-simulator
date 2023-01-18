#ifndef CLOCKABLE_H_
#define CLOCKABLE_H_

/* The Clockable objects are the start and end of the set chain.
 * For the set chain to work properly it is important that:
 *   1. clock() is called for all clockable objects
 *   2. start() is called for all clockable objects
 * It is very important that all object are clocked before any objects are
 * started. This is to ensure that the updated state for each clockable object
 * depends on the last clock cycle.
 */

class Clockable {
public:
    Clockable() = default;
    virtual ~Clockable() = default;
    virtual void clock() = 0;
    virtual void start() = 0;

};

#endif  // CLOCKABLE_H_

