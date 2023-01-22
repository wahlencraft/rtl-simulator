#ifndef WIRE_H_
#define WIRE_H_

#include <list>
#include <cassert>

#include "component.h"
#include "input_port.h"
#include "bit_vector.h"

template <int N>
class Wire : public Entity {
public:
    Wire(std::string const &name="Wire"): Entity(name), target_list{} {};
    Wire(InputPort<N> *target, std::string const &name="Wire"): Entity(name), target_list{target} {}
    Wire(std::initializer_list<InputPort<N>*> lst, std::string const &name="Wire"): Entity(name), target_list{lst} {}

    void add_targets(InputPort<N>* item) {
        target_list.push_back(item);
    }
    void add_targets(std::initializer_list<InputPort<N>*> lst) {
        for (auto item : lst) {
            target_list.push_back(item);
        }
    }

    void reset() override {
        //std::cout << "Resetting " << name << std::endl;
        is_set = false;
        for (auto const& target : target_list) {
            target->reset();
        }
    }

    int get_width() const { return N; }

    void set(BitVector<N> val) {
        //std::cout << "Setting " << name << "=" << val<< std::endl;
        if (is_set) {
            throw std::runtime_error(name + " has alredy been set");
        }
        is_set = true;
        BitVector<N> value = val & MASK;
        if (val != value) {
            std::cout << "Warning: wire not wide enough for value" << std::endl;
        }

        for (auto const &target : target_list) {
            //std::cout << "passing " << value << " to " << target->get_name() << std::endl;
            target->set(value);
        }
    }

private:
    bool is_set = false;
    BitVector<N> const MASK = static_cast<BitVector<N>>((1 << N) - 1);
    std::list<InputPort<N>*> target_list;
};

#endif  // WIRE_H_

