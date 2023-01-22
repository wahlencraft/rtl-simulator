#include <iostream>
#include <chrono>

#include "wire.h"
#include "adder.h"
#include "register.h"
#include "constant.h"
#include "simple_components.h"
#include "sink.h"
#include "clock.h"

using namespace std;

int main() {

    // c0 = 0, c1 = 1, c2 = 0, c3 = 1
    // r0 = 25, r1 = 25, r2 = 0; r3 = 0;
    //                                            c1
    //                                            |w4
    //    |----|                    |----|       _|_
    //    |   _|_                   |   _|_      \ / i0
    //    |  |>  |r0                |  |>  |r1    O
    //    |    |    c0              |    |        |
    //    |    |w0  |w1             |    |w2  |---|w3
    //    |    |    |               |    |    |
    //    |  \-A-\/-B-/             |  \-A-\/-B-/
    //    |   \  a0  Cin------c2    |   \  a1  Cin------ c3
    //    |    \    /     w5        |    \    /     w6
    //    |     ----                |     ----
    //    |       |                 |       |
    //    --------* w7              --------* w8
    //            |                         |
    //           _|_                       _|_
    //          |>  |r2                   |>  |r3
    //            |                         |
    //            |                         |w10
    //            | w9                     _|_
    //            |                        \ /i1
    //            |                         O
    //            |                         |
    //            *---------                |w11
    //            |        |                |
    //            | |------|-*---------------
    //           _|_|_    _|_|_
    //          |  OR |  | XOR |
    //             |        |
    //             |w12     |w13
    //             |        |
    //             s0       s1

    // Declare all wires
    Wire<8> w0{"Wire0"};
    Wire<8> w1{"Wire1"};
    Wire<8> w2{"Wire2"};
    Wire<8> w3{"Wire3"};
    Wire<8> w4{"Wire4"};
    Wire<1> w5{"Wire5"};
    Wire<1> w6{"Wire6"};
    Wire<8> w7{"Wire7"};
    Wire<8> w8{"Wire8"};
    Wire<8> w9{"Wire9"};
    Wire<8> w10{"Wire10"};
    Wire<8> w11{"Wire11"};
    Wire<8> w12{"Wire12"};
    Wire<8> w13{"Wire13"};

    // Pipeline stage 1: Clockables
    Register<8> r0{25, &w0, "Register0"};
    Register<8> r1{25, &w2, "Register1"};
    Constant<8> c0{1, &w1};
    Constant<8> c1{1, &w4};
    Constant<1> c2{0, &w5};
    Constant<1> c3{1, &w6};

    // Pipeline stage 1: Other components
    Adder<8> a0{&w7, "Adder0"};
    Adder<8> a1{&w8, "Adder1"};
    Inverter<8> i0{&w3, "Inverter0"};

    // Pipeline stage 2: Clockables
    Register<8> r2{&w9, "Register2"};
    Register<8> r3{&w10, "Register3"};

    // Pipeline stage 2: Other components
    Inverter<8> i1{&w11, "Inverter1"};
    ORGate<8> OR{&w12, "ORGate"};
    XORGate<8> XOR{&w13, "XORGate"};
    Sink<8> s0{"Sink0"};
    Sink<8> s1{"Sink1"};

    // Add wire targets
    w0.add_targets(&a0.A);
    w1.add_targets(&a0.B);
    w2.add_targets(&a1.A);
    w3.add_targets(&a1.B);
    w4.add_targets(&i0.input);
    w5.add_targets(&a0.Cin);
    w6.add_targets(&a1.Cin);
    w7.add_targets({&r0.input, &r2.input});
    w8.add_targets({&r1.input, &r3.input});
    w9.add_targets({&OR.input[0], &XOR.input[0]});
    w10.add_targets(&i1.input);
    w11.add_targets({&OR.input[1], &XOR.input[1]});
    w12.add_targets(&s0.input);
    w13.add_targets(&s1.input);

    unsigned long iterations = 100000;

    Clock system_clock{4, {&r0, &r1, &r2, &r3, &c0, &c1, &c2, &c3}};

    auto start = std::chrono::high_resolution_clock::now();
    for (unsigned long i=0; i<iterations; ++i) {
        system_clock.clock();
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time taken in total: " << duration.count() << " microseconds" << std::endl;
    std::cout << "Time taken per cycle: " << duration.count()/iterations << " microseconds" << std::endl;
    std::cout << "That is: " << static_cast<double>(iterations) * 1000 / duration.count() << " KHz" << std::endl;

    return 0;
}
