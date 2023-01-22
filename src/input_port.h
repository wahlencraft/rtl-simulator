#ifndef INPUT_PORT_H_
#define INPUT_PORT_H_

#include <iostream>

#include "component.h"
#include "bit_vector.h"

template<int N>
class InputPort: public Entity {
public:
    InputPort(Component *parent=nullptr, std::string const &name="InputPort"): Entity(name), parent{parent} {};
    InputPort(InputPort const& other) = delete;
    InputPort &operator=(InputPort const& other) {
        parent = other.parent;
        value = other.value;
        is_set = other.is_set;
        return *this;
    }

    void set(BitVector<N> val) {
        if (is_set) {
            throw std::runtime_error(name + " has alredy been set");
        }
        is_set = true;
        value = val;
        parent->set();
    }

    void reset() override {
        //std::cout << "Reseting " << name << std::endl;
        is_set = false;
        parent->reset();
    }
    BitVector<N> get_value() { return value; }

private:
    BitVector<N> value{};
    bool is_set{false};
    Component *parent;
};

#endif  // INPUT_PORT_H_

