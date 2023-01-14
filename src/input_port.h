#ifndef INPUT_PORT_H_
#define INPUT_PORT_H_

#include <iostream>

#include "component.h"


template<int N>
#if N <= 8
#   define T uint8_t
#elif N <= 16
#   define T uint16_t
#elif N <= 32
#   define T uint32_t
#elif N <= 64
#   define T uint64_t
#endif
class InputPort: public Entity {
public:
    InputPort(Component *parent, std::string const &name="InputPort"): Entity(name), parent{parent} {};

    void set(T val) {
        std::cout << "Setting " << name << " to: " << unsigned(val) << std::endl;
        if (is_set) {
            throw std::runtime_error(name + " has alredy been set");
        }
        is_set = true;
        value = val;
        parent->set();
    }

    void reset() override {
        std::cout << "Reseting " << name << std::endl;
        is_set = false;
        parent->reset();
    }
    T get_value() { return value; }

private:
    T value{};
    bool is_set{false};
    Component *parent;
};

#endif  // INPUT_PORT_H_

