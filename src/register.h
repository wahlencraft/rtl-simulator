#ifndef REGISTER_H_
#define REGISTER_H_

#include <string>

#include "entity.h"
#include "wire.h"

template <int N>
#if N <= 8
#   define T uint8_t
#elif N <= 16
#   define T uint16_t
#elif N <= 32
#   define T uint32_t
#elif N <= 64
#   define T uint64_t
#endif
class Register : public Component {
public:
    Register(std::string const &name="Register"): Component(name), outwire{nullptr} {}
    Register(Wire<N> *outwire, std::string const &name="Register"): Component(name), outwire{outwire} {}

    InputPort<N> in{this, name + ".in"};

    // Starts the reset chain
    void reset() override {
        if (is_set) {
            // Registers will be the first to be reset (called by the
            // simulator) and will start the reset chain. However the
            // last wire will call upon the InputPort of a Register (which does
            // not know that it's parent is a Register), so each register will
            // be reset twice. It is important to not continnue the reset flow
            // this second time.
            is_set = false;
            if (outwire != nullptr)
                outwire->reset();
        }
    }

    // Setting a Register ends the set chain.
    // To start a set chain, call clock().
    void set() override {
        if (is_set) {
            throw std::runtime_error(name + " has already been set");
        } else {
            is_set = true;
        }
    }

    // Starts the set chain
    void clock() {
        outvalue = in.get_value();
        if (outwire != nullptr) {
            outwire->set(outvalue);
        }
    }

    T get_value() {
        return outvalue;
    }

private:
    Wire<N> *outwire;
    bool is_set{false};
    T outvalue{};
};

#endif  // REGISTER_H_
