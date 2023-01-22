#define CATCH_CONFIG_ENABLE_BENCHMARKING

#include "catch.hpp"

#include "wire.h"
#include "adder.h"
#include "register.h"
#include "constant.h"
#include "bit_vector.h"
#include "sink.h"
#include "simple_components.h"
#include "clock.h"

using namespace std;

TEST_CASE( "BitVectors" ) {
    SECTION( "Constructor, copy, move" ) {
        // Default constructor
        BitVector<10> v0{};
        REQUIRE(v0.length == 10);
        BitVector<7> v1{};
        REQUIRE(v1.length == 7);

        // Construct with value
        BitVector<10> v2{0x20f};
        REQUIRE(v2.length == 10);
        REQUIRE(v2.get_value() == 0x20f);
        uint16_t value3 = 0xfab;
        BitVector<12> v3{value3};
        REQUIRE(v3.get_value() == 0xfab);
        BitVector<18> v18{0x1123};
        REQUIRE(v18.get_value() == 0x1123);
        BitVector<60> v60{0xfffffffffffff0f};
        REQUIRE(v60.get_value() == 0xfffffffffffff0f);

        // Set value
        BitVector<8> v4{};
        v4 = 0xaa;
        REQUIRE(v4.get_value() == 0xaa);
        uint8_t value4 = 0xbb;
        v4 = value4;
        REQUIRE(v4.get_value() == 0xbb);

        // Copy constructor
        BitVector<4> org0{0x3};
        BitVector<4> cpy0{org0};
        REQUIRE(cpy0.get_value() == 0x3);
        BitVector<18> org1{0x1123};
        BitVector<18> cpy1{org1};
        REQUIRE(cpy1.get_value() == 0x1123);

        // Move
        BitVector<16> org3{0xbeef};
        BitVector<16> mov0{std::move(org3)};
        REQUIRE(mov0.get_value() == 0xbeef);
        BitVector<16> org4{0x8008};
        BitVector<16> mov1{};
        mov1 = std::move(org4);
        REQUIRE(mov1.get_value() == 0x8008);
    }

    SECTION( "Comparisons" ) {
        BitVector<10> v0{0x3ff};
        BitVector<10> v1{0x30f};

        // Compare with BitVector
        REQUIRE(v0 != v1);
        REQUIRE_FALSE(v0 == v1);
        v0 = v1;
        REQUIRE(v0 == v1);

        // Compare with constant
        BitVector<8> v3{0xaf};
        REQUIRE(v3 == 0xaf);
        REQUIRE(v3 != 0xff);
    }

    SECTION("Access bits, slicing, concatenation") {
        // Access bit
        BitVector<8> v0{0x74};
        REQUIRE(v0[0] == 0);
        REQUIRE(v0[2] == 1);
        REQUIRE(v0[6] == 1);

        // Slicing
        BitVector<5> v1 = v0.slice<4, 0>();
        REQUIRE(v1 == 0x14);
        BitVector<2> v2 = v0.slice<7, 6>();
        REQUIRE(v2 == 0x1);
        REQUIRE(v0.slice<5, 0>() == 0x34);

        // Concatenation
        BitVector<5> v3{0b00111};
        BitVector<6> v4{0b101000};
        BitVector<3> v5{0b100};
        BitVector<3> v6{0b101};
        REQUIRE( concatenate(v3, v4) == 0b00111101000 );
        REQUIRE( concatenate(v3, v4, v5) == 0b00111101000100 );
        REQUIRE( concatenate(v3, v4, v5, v6) == 0b00111101000100101 );

        // All in one
        BitVector<12> vA{0x30f};
        BitVector<8> vB{0x80};
        BitVector<2> vC{0x3};
        REQUIRE( concatenate( vA.slice<9, 8>(), vA.slice<3, 0>(), vB[7], vC)
                == 0x1ff );
    }

    SECTION( "Extend, signextend" ) {
        BitVector<8> pos8{0x40};  // msb = 0
        BitVector<8> neg8{0x80};  // msb = 1
        BitVector<7> pos7{0x20};  // msb = 0
        BitVector<7> neg7{0x40};  // msb = 1

        CHECK( bitmask<7>() == 0x7f );
        CHECK( bitmask<8>() == 0xff );
        // Extend
        CHECK( pos8.extend<12>() == 0x040 );
        CHECK( neg8.extend<12>() == 0x080 );

        CHECK( pos7.extend<12>() == 0x020 );
        CHECK( neg7.extend<12>() == 0x040 );

        // Signextend
        CHECK( pos8.signextend<12>() == 0x040 );
        CHECK( neg8.signextend<12>() == 0xf80 );

        CHECK( pos7.signextend<12>() == 0x020 );
        CHECK( neg7.signextend<12>() == 0xfc0 );
    }

    SECTION( "Bit Manipulation" ) {
        BitVector<4> a{0b1101};
        BitVector<4> b{0b0110};

        CHECK( ~a == 0b0010 );
        CHECK( (a | b) == 0b1111 );
        CHECK( (a & b) == 0b0100 );
        CHECK( (a ^ b) == 0b1011 );
    }

    SECTION( "Arithmetics" ) {
        BitVector<1> const Cin_1{1};
        BitVector<1> const Cin_0{0};

        // Addition
        BitVector<8> A0{120};
        BitVector<8> B0{11};

        CHECK( A0.add(B0) == 131 );
        CHECK( A0.add(B0, Cin_1) == 132 );

        // With Carry Out
        BitVector<8> A1{0xf0};
        BitVector<8> B1{0x0f};
        CHECK( A1.addc(B1, Cin_0) == 0xff );
        CHECK( A1.addc(B1) == 0xff );
        CHECK( A1.addc(B1, Cin_1) == 0x100 );

        // Inversion
        A0 = 120;
        CHECK( A0.neg() == -120 );

        // Add with negative numbers
        A0 = -110;
        B0 = 120;
        REQUIRE( A0.add(B0) == 10 );
        REQUIRE( A0.add(B0, Cin_0) == 10 );
        REQUIRE( A0.addc(B0) == 10 + (1<<8));
        REQUIRE( A0.addc(B0, Cin_0) == 10 + (1<<8));

        A0 = 0x50;
        B0 = 0x92;
        REQUIRE( A0.add(B0) == 0xe2 );

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
        CHECK( r0.get_value() == 0 );

        Register<8> r1{0x11};
        CHECK( r1.get_value() == 0x11 );

        Wire<16> w0{};
        Register<16> r2{&w0};

        Wire<16> w1{};
        Register<16> r3{0x22, &w1};
        CHECK( r3.get_value() == 0x22 );
    }

    SECTION( "Set, clock, start and reset" ) {
        Register<16> r{};
        CHECK_NOTHROW( r.input.set(23) );
        CHECK_NOTHROW( r.clock() );
        CHECK_NOTHROW( r.start_set_chain() );
        CHECK_NOTHROW( r.input.reset() );

        CHECK_NOTHROW( r.input.set(24) );
        CHECK_THROWS( r.input.set(24) );
    }

    SECTION( "Set, clock, start and get_value" ) {
        Sink<16> sink{};
        Wire<16> wire{&sink.input};
        Register<16> r{&wire};
        r.input.set(23);
        r.clock();
        r.start_set_chain();
        r.input.reset();
        r.start_reset_chain();

        CHECK( r.get_value() == 23 );
        r.input.set(24);
        CHECK( r.get_value() == 23 );
        r.clock();
        CHECK( sink.get_value() == 23 );
        CHECK( r.get_value() == 24 );
        r.start_set_chain();
        CHECK( sink.get_value() == 24 );
    }
}

TEST_CASE( "Constants" ) {
    Sink<6> s{};
    Wire<6> w{&s.input};
    Constant<6> c{0x24, &w};

    c.start_set_chain();
    CHECK( s.get_value() == 0x24 );
}

TEST_CASE( "Simple Components" ) {
    SECTION( "Inverter" ) {
        Sink<4> sink{};
        Wire<4> wire{&sink.input};
        Inverter<4> inverter{&wire};

        inverter.input.set(0b1101);
        CHECK( sink.get_value() == 0b0010 );
    }
    SECTION( "AND Gate" ) {
        Sink<4> sink{};
        Wire<4> wire{&sink.input};
        ANDGate<4> gate{&wire};

        gate.input[0].set(0b0011);
        gate.input[1].set(0b0101);
        CHECK( sink.get_value() == 0b0001 );
    }
    SECTION( "NAND Gate" ) {
        Sink<4> sink{};
        Wire<4> wire{&sink.input};
        NANDGate<4> gate{&wire};

        gate.input[0].set(0b0011);
        gate.input[1].set(0b0101);
        CHECK( sink.get_value() == 0b1110 );
    }
    SECTION( "OR Gate" ) {
        Sink<4> sink{};
        Wire<4> wire{&sink.input};
        ORGate<4> gate{&wire};

        gate.input[0].set(0b0011);
        gate.input[1].set(0b0101);
        CHECK( sink.get_value() == 0b0111 );
    }
    SECTION( "XOR Gate" ) {
        Sink<4> sink{};
        Wire<4> wire{&sink.input};
        XORGate<4> gate{&wire};

        gate.input[0].set(0b0011);
        gate.input[1].set(0b0101);
        CHECK( sink.get_value() == 0b0110 );
    }
    SECTION( "NOR Gate" ) {
        Sink<4> sink{};
        Wire<4> wire{&sink.input};
        NORGate<4> gate{&wire};

        gate.input[0].set(0b0011);
        gate.input[1].set(0b0101);
        CHECK( sink.get_value() == 0b1000 );
    }
}

TEST_CASE( "Adder" ) {
    //
    //           \-A-\/-B-/
    //    |-------\      Cin
    //    |        \    /
    //    |         ----
    //    |           |
    //    |           |
    //   _|_         _|_
    //  |>  | cout  |>  | result
    //
    Register<8> result8{};
    Wire<8> w_result8{&result8.input};
    Register<1> cout8{};
    Wire<1> w_cout8{&cout8.input};
    Adder<8> adder8{&w_cout8, &w_result8};

    ///////////////////////////////////////////////////////////////////////////
    // Basic test

    adder8.A.set(0x05);
    adder8.B.set(0x03);
    adder8.Cin.set(0x01);

    cout8.clock();
    result8.clock();

    CHECK( result8.get_value() == 0x09 );
    CHECK( cout8.get_value() == 0x0 );

    adder8.A.reset();
    adder8.B.reset();
    adder8.Cin.reset();

    ///////////////////////////////////////////////////////////////////////////
    // Overflow

    adder8.A.set(251);
    adder8.B.set(6);
    adder8.Cin.set(0);

    cout8.clock();
    result8.clock();

    CHECK( result8.get_value() == 0x1 );
    CHECK( cout8.get_value() == 0x1 );

    adder8.A.reset();
    adder8.B.reset();
    adder8.Cin.reset();

    ///////////////////////////////////////////////////////////////////////////
    // Subtraction

    BitVector<8> val1 = 12;
    BitVector<8> val2 = 11;

    adder8.A.set(val1);
    adder8.B.set(~val2);
    adder8.Cin.set(1);

    cout8.clock();
    result8.clock();

    CHECK( result8.get_value() == 0x1 );

    adder8.A.reset();
    adder8.B.reset();
    adder8.Cin.reset();

}

TEST_CASE( "Constallation 1: Registers, adders and wires") {
    //
    //    r0    r1                  r2
    //    ___   ___                 ___
    //   |>  | |>  |               |>  |
    //     |    |                    |
    //     |w0  |w1                  |w2
    //     |    |                    |
    //     |    |---------------|    |             ___
    //     |    |               |    |            |>  | r3
    //   \-A-\/-B-/     w3    \-A-\/-B-/    w4      |
    //    \  a0  Cin-----------\  a1  Cin------------
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

    Wire<8> w5{&r4.input, "Wire5"};
    Wire<8> w6{&r5.input, "Wire6"};

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

        r0.input.set(7);
        r1.input.set(8);
        r2.input.set(15);
        r3.input.set(0);

        cout << "\n";

        r0.clock();
        r1.clock();
        r2.clock();
        r3.clock();
        r4.clock();
        r5.clock();

        r0.start_set_chain();
        r1.start_set_chain();
        r2.start_set_chain();
        r3.start_set_chain();
        r4.start_set_chain();
        r5.start_set_chain();

        cout << "\n";

        r0.clock();
        r1.clock();
        r2.clock();
        r3.clock();
        r4.clock();
        r5.clock();

        REQUIRE( r5.get_value() == 23 );
        REQUIRE( r4.get_value() == 15 );

        r0.input.reset();
        r1.input.reset();
        r2.input.reset();
        r3.input.reset();
    }

    SECTION( "Carry in" ) {

        r0.input.set(0);
        r1.input.set(8);
        r2.input.set(0);
        r3.input.set(1);

        cout << "\n";

        r0.clock();
        r1.clock();
        r2.clock();
        r3.clock();
        r4.clock();
        r5.clock();

        r0.start_set_chain();
        r1.start_set_chain();
        r2.start_set_chain();
        r3.start_set_chain();
        r4.start_set_chain();
        r5.start_set_chain();

        cout << "\n";

        r0.clock();
        r1.clock();
        r2.clock();
        r3.clock();
        r4.clock();
        r5.clock();

        REQUIRE( r5.get_value() == 9 );
        REQUIRE( r4.get_value() == 8 );

        r0.input.reset();
        r1.input.reset();
        r2.input.reset();
        r3.input.reset();
    }

    SECTION( "Overflow" ) {

        r0.input.set(0);
        r1.input.set(0x1);
        r2.input.set(0xff);
        r3.input.set(0);

        cout << "\n";

        r0.clock();
        r1.clock();
        r2.clock();
        r3.clock();
        r4.clock();
        r5.clock();

        r0.start_set_chain();
        r1.start_set_chain();
        r2.start_set_chain();
        r3.start_set_chain();
        r4.start_set_chain();
        r5.start_set_chain();

        cout << "\n";

        r0.clock();
        r1.clock();
        r2.clock();
        r3.clock();
        r4.clock();
        r5.clock();

        REQUIRE( r5.get_value() == 0 );
        REQUIRE( r4.get_value() == 2 );

        r0.input.reset();
        r1.input.reset();
        r2.input.reset();
        r3.input.reset();
    }

    SECTION( "Negative" ) {

        r0.input.set(-110);
        r1.input.set(80);
        r2.input.set(120);
        r3.input.set(0);

        cout << "\n";

        r0.clock();
        r1.clock();
        r2.clock();
        r3.clock();
        r4.clock();
        r5.clock();

        r0.start_set_chain();
        r1.start_set_chain();
        r2.start_set_chain();
        r3.start_set_chain();
        r4.start_set_chain();
        r5.start_set_chain();

        cout << "\n";

        r0.clock();
        r1.clock();
        r2.clock();
        r3.clock();
        r4.clock();
        r5.clock();

        CHECK( r5.get_value() == 200 );
        CHECK( r4.get_value() == -30 );

        r0.input.reset();
        r1.input.reset();
        r2.input.reset();
        r3.input.reset();
    }
}

TEST_CASE( "Constallation 2: Owned by Clock") {

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

    SECTION("Check output") {
        // Make the clock
        Clock system_clock{20, {&r0, &r1, &r2, &r3, &c0, &c1, &c2, &c3}};

        // Tests
        cout << "\nTest Clock" << endl;
        system_clock.clock();

        CHECK( r0.get_value() == 26 );
        CHECK( r1.get_value() == 24 );
        CHECK( r2.get_value() == 26 );
        CHECK( r3.get_value() == 24 );
        CHECK( s0.get_value() == (0 | ~0) );
        CHECK( s1.get_value() == (0 ^ ~0) );

        system_clock.clock();
        CHECK( r0.get_value() == 27 );
        CHECK( r1.get_value() == 23 );
        CHECK( r2.get_value() == 27 );
        CHECK( r3.get_value() == 23 );
        CHECK( s0.get_value() == (26 | ~24) );
        CHECK( s1.get_value() == (26 ^ ~24) );
    }
    BENCHMARK_ADVANCED("1 Thread, no Clock (manual clocking)")(Catch::Benchmark::Chronometer meter) {
        meter.measure([&r0, &r1, &r2, &r3, &c0, &c1, &c2, &c3] {
            r0.start_set_chain();
            r1.start_set_chain();
            r2.start_set_chain();
            r3.start_set_chain();
            c0.start_set_chain();
            c1.start_set_chain();
            c2.start_set_chain();
            c3.start_set_chain();

            r0.start_reset_chain();
            r1.start_reset_chain();
            r2.start_reset_chain();
            r3.start_reset_chain();
            c0.start_reset_chain();
            c1.start_reset_chain();
            c2.start_reset_chain();
            c3.start_reset_chain();

            r0.clock();
            r1.clock();
            r2.clock();
            r3.clock();
            c0.clock();
            c1.clock();
            c2.clock();
            c3.clock();
        });
    };

    BENCHMARK_ADVANCED("1 Thread")(Catch::Benchmark::Chronometer meter) {
        Clock system_clock{1, {&r0, &r1, &r2, &r3, &c0, &c1, &c2, &c3}};
        meter.measure([&system_clock] { return system_clock.clock(); });
    };

    BENCHMARK_ADVANCED("2 Threads")(Catch::Benchmark::Chronometer meter) {
        Clock system_clock{2, {&r0, &r1, &r2, &r3, &c0, &c1, &c2, &c3}};
        meter.measure([&system_clock] { return system_clock.clock(); });
    };

    BENCHMARK_ADVANCED("4 Threads")(Catch::Benchmark::Chronometer meter) {
        Clock system_clock{2, {&r0, &r1, &r2, &r3, &c0, &c1, &c2, &c3}};
        meter.measure([&system_clock] { return system_clock.clock(); });
    };

    BENCHMARK_ADVANCED("Max Threads")(Catch::Benchmark::Chronometer meter) {
        Clock system_clock{0, {&r0, &r1, &r2, &r3, &c0, &c1, &c2, &c3}};
        meter.measure([&system_clock] { return system_clock.clock(); });
    };
}

