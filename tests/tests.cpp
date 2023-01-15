#include "catch.hpp"

#include "wire.h"
#include "adder.h"
#include "register.h"
#include "bit_vector.h"

using namespace std;

TEST_CASE( "BitVectors" ) {
    SECTION( "Constructors, comparitions, copy" ) {
        BitVector<10> v0{};
        REQUIRE(v0.length == 10);

        BitVector<10> v1{0x20f};
        REQUIRE(v0 != v1);
        REQUIRE_FALSE(v0 == v1);
        v0 = v1;
        REQUIRE(v0 == v1);
        BitVector<10> v2{v0};
        REQUIRE(v0 == v2);
        v0 = 0x1ff;
        REQUIRE_FALSE(v0 == v2);
        REQUIRE(v0 == 0x1ff);
        REQUIRE_FALSE(v0 == 0x1f7);

        BitVector<4> v3{0x82};
        REQUIRE(v3 == 0x2);
        v3 = 0x44;
        REQUIRE(v3 == 0x4);
    }
    SECTION("Access bits, slicing, concatenation") {
        BitVector<8> v0{0x74};
        REQUIRE(v0[0] == 0);
        REQUIRE(v0[2] == 1);
        REQUIRE(v0[6] == 1);
        BitVector<5> v1 = v0.slice<4, 0>();
        REQUIRE(v1 == 0x14);
        BitVector<2> v2 = v0.slice<7, 6>();
        REQUIRE(v2 == 0x1);

        BitVector<5> v3{0b00111};
        BitVector<9> v4{0b101000100};
        REQUIRE( concatenate(v3, v4) == 0b00111101000100 );
    }
}

TEST_CASE( "Wires" ) {
    SECTION( "Set and reset" ) {
        Wire<10> w{};
        CHECK_NOTHROW( w.set(1) );
        CHECK_THROWS( w.set(2) );
        w.reset();
        CHECK_NOTHROW( w.set(1) );
        w.reset();
        CHECK_NOTHROW( w.set(2) );
    }
}

TEST_CASE( "Registers" ) {
    SECTION( "Constructors" ) {
        Register<8> r0{};

        Wire<16> w{};
        Register<16> r1{&w};
    }

    SECTION( "Set, clock and reset" ) {
        Register<16> r{};
        CHECK_NOTHROW( r.in.set(23) );
        CHECK_NOTHROW( r.clock() );
        CHECK_NOTHROW( r.in.reset() );

        CHECK_NOTHROW( r.in.set(24) );
        CHECK_THROWS( r.in.set(24) );
    }

    SECTION( "Set, clock and get_value" ) {
        Register<16> r{};
        r.in.set(23);
        r.clock();
        r.in.reset();

        REQUIRE( r.get_value() == 23 );
        r.in.set(24);
        REQUIRE( r.get_value() == 23 );
        r.clock();
        REQUIRE( r.get_value() == 24 );
    }
}

TEST_CASE( "Constallation 1: Registers, adders and wires") {
    //
    //    r0    r1                  r2
    //    ___   ___                 ___
    //   |>  | |>  |               |>  |
    //     |    |                    |
    //     |w0  |w1                  |w2
    //     |    |                    |
    //     |    |---------------|    |
    //     |    |               |    |
    //   \-A-\/-B-/     w3    \-A-\/-B-/    w4   ___
    //    \  a0  Cin-----------\  a1  Cin-------|>  | r3
    //     \    /               \    /
    //      ----                 ----
    //        |                    |
    //        |w5                  |w6
    //        |                    |
    //       _|_                  _|_
    //      |>  | r4             |>  | r5
    //
    Register<8> r4{"Register4"};
    Register<8> r5{"Register5"};

    Wire<8> w5{&r4.in, "Wire5"};
    Wire<8> w6{&r5.in, "Wire6"};

    Adder<8> a0{&w5, "Adder0"};
    Wire<8> w0{{&a0.A}, "Wire0"};

    Wire<1> w3{&a0.Cin, "Wire3"};
    Adder<8> a1{&w3, &w6, "Adder1"};

    Wire<1> w4{&a1.Cin, "Wire4"};
    Register<1> r3{&w4};

    Wire<8> w2{&a1.B, "Wire2"};
    Wire<8> w1{{&a0.B, &a1.A}, "Wire1"};

    Register<8> r2{&w2, "Register2"};
    Register<8> r1{&w1, "Register1"};
    Register<8> r0{&w0, "Register0"};

    SECTION( "Simple test" ) {

        r0.in.set(7);
        r1.in.set(8);
        r2.in.set(15);
        r3.in.set(0);

        cout << "\n";

        r0.clock();
        r1.clock();
        r2.clock();
        r3.clock();

        cout << "\n";

        r4.clock();
        r5.clock();

        REQUIRE( r5.get_value() == 23 );
        REQUIRE( r4.get_value() == 15 );

        r0.in.reset();
        r1.in.reset();
        r2.in.reset();
        r3.in.reset();
    }

    SECTION( "Carry in" ) {

        r0.in.set(0);
        r1.in.set(8);
        r2.in.set(0);
        r3.in.set(1);

        cout << "\n";

        r0.clock();
        r1.clock();
        r2.clock();
        r3.clock();

        cout << "\n";

        r4.clock();
        r5.clock();

        REQUIRE( r5.get_value() == 9 );
        REQUIRE( r4.get_value() == 8 );

        r0.in.reset();
        r1.in.reset();
        r2.in.reset();
        r3.in.reset();
    }

    SECTION( "Carry out" ) {

        r0.in.set(0);
        r1.in.set(0x1);
        r2.in.set(0xff);
        r3.in.set(0);

        cout << "\n";

        r0.clock();
        r1.clock();
        r2.clock();
        r3.clock();

        cout << "\n";

        r4.clock();
        r5.clock();

        REQUIRE( r5.get_value() == 0 );
        REQUIRE( r4.get_value() == 2 );

        r0.in.reset();
        r1.in.reset();
        r2.in.reset();
        r3.in.reset();
    }

    SECTION( "Negative" ) {

        r0.in.set(80);
        r1.in.set(-110);
        r2.in.set(120);
        r3.in.set(0);

        cout << "\n";

        r0.clock();
        r1.clock();
        r2.clock();
        r3.clock();

        cout << "\n";

        r4.clock();
        r5.clock();

        REQUIRE( r5.get_value() == 10 );
        REQUIRE( r4.get_value() == -30 );

        r0.in.reset();
        r1.in.reset();
        r2.in.reset();
        r3.in.reset();
    }
}
