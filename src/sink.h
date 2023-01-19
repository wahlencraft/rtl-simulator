#ifndef SINK_H_
#define SINK_H_

#include <string>

#include "entity.h"
#include "bit_vector.h"
#include "input_port.h"

// The Sink class is used mostly for debugging.
// It remembers the last value that it was set to.

template <int N>
class Sink : public Component {
public:
    Sink(std::string const &name="Sink"): Component(name) {};
    BitVector<N> get_value() const {
        return value;
    }
    InputPort<N> input{this, name+".in"};

    void set() override {
        if (is_set) {
            throw std::runtime_error(name + " has already been set");
        } else {
            is_set = true;
            value = input.get_value();
        }
    }
    void reset() override {
        is_set = false;
    }

private:
    BitVector<N> value{};
    bool is_set = false;
};

#endif  // SINK_H_

