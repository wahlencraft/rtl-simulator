#ifndef CONSTANT_H_
#define CONSTANT_H_

#include <string>

#include "bit_vector.h"
#include "wire.h"

/* Constants requires a value when constructed and keeps that value. */

template <int N>
class Constant: public Clockable {
public:
    Constant(BitVector<N> value, Wire<N> *outwire): Clockable(), value{value}, outwire{outwire} {}
    Constant(Constant<N> const &) = delete;
    void operator=(Constant<N> const &) = delete;
    void clock() override {}
    void start() override {
        outwire->set(value);
    }

private:
    BitVector<N> const value;
    Wire<N> *outwire;
};

#endif  // CONSTANT_H_

