#ifndef WIRE_H_
#define WIRE_H_

#include <list>
#include <cassert>

#include "component.h"
#include "port.h"

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
class Wire : public Entity {
public:
    Wire(std::string name="Wire"): Entity(name), target_list{} {};
    Wire(InputPort<N> *target, std::string name="Wire"): Entity(name), target_list{target} {}
    Wire(std::initializer_list<InputPort<N>*> lst, std::string name="Wire"): Entity(name), target_list{lst} {}

    void reset() override {
        std::cout << "Resetting " << name << std::endl;
        is_set = false;
        for (auto const& target : target_list) {
            target->reset();
        }
    }

    int get_width() const { return N; }

    void set(T val) {
        assert(is_set == false);
        is_set = true;
        T value = val & MASK;
        if (val != value) {
            std::cout << "Warning: wire not wide enough for value" << std::endl;
        }
        std::cout << "set " << name << " to: '" << unsigned(value) << "'" << std::endl;

        for (auto const &target : target_list) {
            std::cout << "passing value to component..." << std::endl;
            target->set(value);
        }
    }

private:
    bool is_set = false;
    T const MASK = static_cast<T>((1 << N) - 1);
    std::list<InputPort<N>*> target_list;
};

#endif  // WIRE_H_

