#ifndef REGISTER_H_
#define REGISTER_H_

#include <string>

#include "entity.h"
#include "component.h"
#include "clockable.h"
#include "wire.h"

template <int N>
class Register : public Component, public Clockable {
public:
    Register(std::string const &name="Register"):
        Component(name), Clockable(), outvalue{0}, outwire{nullptr} {}
    Register(Wire<N> *outwire, std::string const &name="Register"):
        Component(name), Clockable(), outvalue{0}, outwire{outwire} {}
    Register(BitVector<N> value, std::string const &name="Register"):
        Component(name), Clockable(), outvalue{value}, outwire{nullptr} {}
    Register(BitVector<N> value, Wire<N> *outwire, std::string const &name="Register"):
        Component(name), Clockable(), outvalue{value}, outwire{outwire} {}
    Register(Register const&) = delete;
    Register operator=(Register const&) = delete;

    InputPort<N> input{this, name + ".in"};

    void start_set_chain() override {
        std::cout << "Starting setchain from " << name << std::endl;
        if (outwire != nullptr) {
            outwire->set(outvalue);
        }
    }

    void start_reset_chain() override {
        if (outwire != nullptr)
            outwire->reset();
    }

    void reset() override {
        // This is the last stage in the reset chain
        is_set = false;
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
    void clock() override {
        std::cout << "clocking " << name << std::endl;
        outvalue = input.get_value();
    }

    BitVector<N> get_value() {
        return outvalue;
    }

private:
    BitVector<N> outvalue;
    Wire<N> *outwire;
    bool is_set{false};
};

#endif  // REGISTER_H_

