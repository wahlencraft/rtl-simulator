#ifndef SIMPLE_COMPONENT_H_
#define SIMPLE_COMPONENT_H_

#include <string>

#include "component.h"
#include "input_port.h"
#include "bit_vector.h"

template <int N>
class SimpleComponent: public Component {
public:
    SimpleComponent(Wire<N> *outwire, std::string const &name="SimpleComponent"): Component(name), outwire{outwire} {}
    SimpleComponent(SimpleComponent const &) = delete;
    void operator=(SimpleComponent<N> const &) = delete;
    InputPort<N> in{this, name + ".in"};

    void set() override {
        if (is_set) {
            throw std::runtime_error(name + " has already been set");
        } else {
            is_set = true;
            BitVector<N> value = calculate_outvalue();
            outwire->set(value);
        }
    }

    void reset() override {
        is_set = false;
        outwire->reset();
    }

protected:
    virtual BitVector<N> calculate_outvalue() = 0;

private:
    Wire<N> *outwire;
    bool is_set{false};
};

template <int N>
class Inverter: public SimpleComponent<N> {
public:
    Inverter(Wire<N> *outwire, std::string const &name="Inverter"): SimpleComponent<N>(outwire, name) {}

private:
    BitVector<N> calculate_outvalue() override {
        return ~(SimpleComponent<N>::in.get_value());
    }
};


#endif  // SIMPLE_COMPONENT_H_

