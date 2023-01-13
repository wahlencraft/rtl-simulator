#ifndef PORT_H_
#define PORT_H_

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
        assert(!been_set);
        been_set = true;
        value = val;
        parent->set();
    }

    void reset() override {
        std::cout << "Reseting " << name << std::endl;
        been_set = false;
        parent->reset();
    }
    T get_value() { return value; }

private:
    T value{};
    bool been_set{false};
    Component *parent;
};

#endif  // PORT_H_

