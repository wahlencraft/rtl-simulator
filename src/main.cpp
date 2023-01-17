#include <iostream>

#include "wire.h"
#include "adder.h"

using namespace std;

template <int N>
class MyClass0 {
public:
    template<int n, int m>
    int my_method() {
        return n + m;
    }
};

template <int N>
class MyClass1 {
public:
    int my_method() {
        MyClass0<N> obj{};
        return obj.template my_method<N, N-1>();
    }
};

int main() {
    MyClass1<5> obj{};
    cout << "Result = " << obj.my_method() << endl;

    //    w0   w1                   w2
    //     |    |                    |
    //     |    |---------------|    |
    //     |    |               |    |
    //   \-A-\/-B-/     w3    \-A-\/-B-/
    //    \  a0  Cin-----------\  a1  Cin--- w4
    //     \    /               \    /
    //      ----                 ----
    //        |                    |
    //        |                    |
    //       w5                   w6

    Wire<8> w5{"Wire5"};
    Wire<8> w6{"Wire6"};

    Adder<8> a0{&w5, "Adder0"};
    Wire<8> w0{{&a0.A}, "Wire0"};

    Wire<1> w3{&a0.Cin, "Wire3"};
    Adder<8> a1{&w3, &w6, "Adder1"};

    Wire<1> w4{&a1.Cin, "Wire4"};
    Wire<8> w2{&a1.B, "Wire2"};
    Wire<8> w1{{&a0.B, &a1.A}, "Wire1"};

    w0.set(7);
    w1.set(8);
    w2.set(15);
    w4.set(0);

    cout << "\n";

    w0.reset();
    w1.reset();
    w2.reset();
    w4.reset();

    cout << "\n";

    w0.set(7);
    w1.set(8);
    w2.set(1);
    w4.set(0);

    return 0;
}
