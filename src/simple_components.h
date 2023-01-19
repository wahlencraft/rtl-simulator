#ifndef SIMPLE_COMPONENTS_H_
#define SIMPLE_COMPONENTS_H_

#include <string>
#include <array>

#include "component.h"
#include "input_port.h"
#include "bit_vector.h"

template <int N, int INPUTS>
class SimpleComponent: public Component {
public:
    SimpleComponent(Wire<N> *outwire, std::string const &name="SimpleComponent"): Component(name), input{}, outwire{outwire} {
        input.fill(InputPort<N>{this, name+".input[]"});
    }
    SimpleComponent(SimpleComponent const &) = delete;
    void operator=(SimpleComponent<N, INPUTS> const &) = delete;

    std::array<InputPort<N>, INPUTS> input;

    void set() override {
        if (++set_count > INPUTS) {
            throw std::runtime_error(name + " has already been set " + std::to_string(INPUTS) + " times");
        } else if (set_count == INPUTS) {
            BitVector<N> value = calculate_outvalue();
            outwire->set(value);
        }
    }

    void reset() override {
        if (set_count) {
            set_count = 0;
            outwire->reset();
        }
    }

protected:
    virtual BitVector<N> calculate_outvalue() = 0;

private:
    Wire<N> *outwire;
    int set_count{0};
};

template <int N>
class Inverter: public SimpleComponent<N, 1> {
public:
    Inverter(Wire<N> *outwire, std::string const &name="Inverter"): SimpleComponent<N, 1>(outwire, name) {}
    InputPort<N> &input = SimpleComponent<N, 1>::input[0];

private:
    BitVector<N> calculate_outvalue() override {
        return ~(input.get_value());
    }
};

template <int N>
class ANDGate: public SimpleComponent<N, 2> {
public:
    ANDGate(Wire<N> *outwire, std::string const &name="AndGate"): SimpleComponent<N, 2>(outwire, name) {}

private:
    BitVector<N> calculate_outvalue() override {
        return this->input[0].get_value() & this->input[1].get_value();
    }
};

template <int N>
class NANDGate: public SimpleComponent<N, 2> {
public:
    NANDGate(Wire<N> *outwire, std::string const &name="AndGate"): SimpleComponent<N, 2>(outwire, name) {}

private:
    BitVector<N> calculate_outvalue() override {
        return ~(this->input[0].get_value() & this->input[1].get_value());
    }
};

template <int N>
class ORGate: public SimpleComponent<N, 2> {
public:
    ORGate(Wire<N> *outwire, std::string const &name="AndGate"): SimpleComponent<N, 2>(outwire, name) {}

private:
    BitVector<N> calculate_outvalue() override {
        return this->input[0].get_value() | this->input[1].get_value();
    }
};

template <int N>
class XORGate: public SimpleComponent<N, 2> {
public:
    XORGate(Wire<N> *outwire, std::string const &name="AndGate"): SimpleComponent<N, 2>(outwire, name) {}

private:
    BitVector<N> calculate_outvalue() override {
        return this->input[0].get_value() ^ this->input[1].get_value();
    }
};

template <int N>
class NORGate: public SimpleComponent<N, 2> {
public:
    NORGate(Wire<N> *outwire, std::string const &name="AndGate"): SimpleComponent<N, 2>(outwire, name) {}

private:
    BitVector<N> calculate_outvalue() override {
        return ~(this->input[0].get_value() | this->input[1].get_value());
    }
};

#endif  // SIMPLE_COMPONENTS_H_

