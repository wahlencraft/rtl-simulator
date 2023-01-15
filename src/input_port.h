#ifndef INPUT_PORT_H_
#define INPUT_PORT_H_

#include <iostream>

#include "component.h"
#include "types.h"

template<int N>
class InputPort: public Entity {
public:
    InputPort(Component *parent, std::string const &name="InputPort"): Entity(name), parent{parent} {};
    InputPort(InputPort const&) = delete;
    InputPort operator=(InputPort const&) = delete;

    void set(bits<N> val) {
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
    bits<N> get_value() { return value; }

private:
    bits<N> value{};
    bool is_set{false};
    Component *parent;
};

#endif  // INPUT_PORT_H_

