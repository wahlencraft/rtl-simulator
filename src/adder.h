#ifndef ADDER_H_
#define ADDER_H_

#include <atomic>

#include "bit_vector.h"

template <int N>
class Adder : public Component {
public:
    Adder(Wire<N> *outwire, std::string const &name="Adder"): Component(name), outwire{outwire}, Cout{nullptr} {}
    Adder(Wire<1> *Cout, Wire<N> *outwire, std::string const &name="Adder"): Component(name), outwire{outwire}, Cout{Cout} {}
    Adder(const Adder &) = delete;
    Adder operator=(const Adder &) = delete;

    InputPort<N> A{this, name + ".A"};
    InputPort<N> B{this, name + ".B"};
    InputPort<1> Cin{this, name + ".Cin"};

    void reset() override {
        // set_count_copy = set_count
        // set_count = 0
        int const set_count_copy = set_count.exchange(0);

        if (set_count_copy) {
            //std::cout << "Reseting " << name << std::endl;
            if (Cout != nullptr)
                Cout->reset();
            outwire->reset();
        }
    }

    void set() override {
        int const set_count_copy = ++set_count;
        if (set_count_copy == 3) {
            //std::cout << "Setting " << name << std::endl;
            // Ready to calculate output

            if (Cout != nullptr) {
                BitVector<N+1> sum = A.get_value().addc(B.get_value(), Cin.get_value());
                outwire->set(sum.template slice<N-1, 0>());
                Cout->set((sum[N]));
            } else {
                // Throw away carry bit
                BitVector<N> sum = A.get_value().add(B.get_value(), Cin.get_value());
                outwire->set(sum);
            }
        } else if (set_count_copy > 3) {
            throw std::runtime_error(name + " has been set too many times");
        }
    }

private:
    Wire<N> *outwire;
    Wire<1> *Cout;
    std::atomic_int set_count = 0;
    BitVector<N+1> const MASK = static_cast<BitVector<N+1>>((1 << N) - 1);
};

#endif  // ADDER_H_

