#ifndef CONSTANT_H_
#define CONSTANT_H_

#include <string>

#include "bit_vector.h"
#include "wire.h"

/* Constants requires a value when constructed and keeps that value. */

template <int N>
class Constant: public Clockable {
public:
    Constant(BitVector<N> value, Wire<N> *outwire): 
        Clockable(), value{value}, outwire{outwire} {}
    Constant(Constant<N> const &) = delete;
    void operator=(Constant<N> const &) = delete;
    void clock() override {
        std::cout << "clocking constant=" << value << std::endl;
    }
    void start_set_chain() override {
        std::cout << "Starting setchain from constant=" << value << std::endl;
        outwire->set(value);
    }
    void start_reset_chain() override {
        outwire->reset();
    }

private:
    BitVector<N> const value;
    Wire<N> *outwire;
};

#endif  // CONSTANT_H_

