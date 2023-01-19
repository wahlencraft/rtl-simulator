#include "catch.hpp"

#include "wire.h"
#include "adder.h"
#include "register.h"
#include "bit_vector.h"
#include "sink.h"
#include "simple_components.h"

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

        Wire<16> w{};
        Register<16> r1{&w};
    }

    SECTION( "Set, clock, start and reset" ) {
        Register<16> r{};
        CHECK_NOTHROW( r.in.set(23) );
        CHECK_NOTHROW( r.clock() );
        CHECK_NOTHROW( r.start() );
        CHECK_NOTHROW( r.in.reset() );

        CHECK_NOTHROW( r.in.set(24) );
        CHECK_THROWS( r.in.set(24) );
    }

    SECTION( "Set, clock, start and get_value" ) {
        Sink<16> sink{};
        Wire<16> wire{&sink.in};
        Register<16> r{&wire};
        r.in.set(23);
        r.clock();
        r.start();
        r.in.reset();

        CHECK( r.get_value() == 23 );
        r.in.set(24);
        CHECK( r.get_value() == 23 );
        r.clock();
        CHECK( sink.get_value() == 23 );
        CHECK( r.get_value() == 24 );
        r.start();
        CHECK( sink.get_value() == 24 );
    }
}

TEST_CASE( "Simple Components" ) {
    SECTION( "Inverter" ) {
        Sink<4> sink{};
        Wire<4> wire{&sink.in};
        Inverter<4> inverter{&wire};

        inverter.input[0].set(0b1101);
        CHECK( sink.get_value() == 0b0010 );
    }
    SECTION( "AND Gate" ) {
        Sink<4> sink{};
        Wire<4> wire{&sink.in};
        ANDGate<4> gate{&wire};

        gate.input[0].set(0b0011);
        gate.input[1].set(0b0101);
        CHECK( sink.get_value() == 0b0001 );
    }
    SECTION( "NAND Gate" ) {
        Sink<4> sink{};
        Wire<4> wire{&sink.in};
        NANDGate<4> gate{&wire};

        gate.input[0].set(0b0011);
        gate.input[1].set(0b0101);
        CHECK( sink.get_value() == 0b1110 );
    }
    SECTION( "OR Gate" ) {
        Sink<4> sink{};
        Wire<4> wire{&sink.in};
        ORGate<4> gate{&wire};

        gate.input[0].set(0b0011);
        gate.input[1].set(0b0101);
        CHECK( sink.get_value() == 0b0111 );
    }
    SECTION( "XOR Gate" ) {
        Sink<4> sink{};
        Wire<4> wire{&sink.in};
        XORGate<4> gate{&wire};

        gate.input[0].set(0b0011);
        gate.input[1].set(0b0101);
        CHECK( sink.get_value() == 0b0110 );
    }
    SECTION( "NOR Gate" ) {
        Sink<4> sink{};
        Wire<4> wire{&sink.in};
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
    Wire<8> w_result8{&result8.in};
    Register<1> cout8{};
    Wire<1> w_cout8{&cout8.in};
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
        r4.clock();
        r5.clock();

        r0.start();
        r1.start();
        r2.start();
        r3.start();
        r4.start();
        r5.start();

        cout << "\n";

        r0.clock();
        r1.clock();
        r2.clock();
        r3.clock();
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
        r4.clock();
        r5.clock();

        r0.start();
        r1.start();
        r2.start();
        r3.start();
        r4.start();
        r5.start();

        cout << "\n";

        r0.clock();
        r1.clock();
        r2.clock();
        r3.clock();
        r4.clock();
        r5.clock();

        REQUIRE( r5.get_value() == 9 );
        REQUIRE( r4.get_value() == 8 );

        r0.in.reset();
        r1.in.reset();
        r2.in.reset();
        r3.in.reset();
    }

    SECTION( "Overflow" ) {

        r0.in.set(0);
        r1.in.set(0x1);
        r2.in.set(0xff);
        r3.in.set(0);

        cout << "\n";

        r0.clock();
        r1.clock();
        r2.clock();
        r3.clock();
        r4.clock();
        r5.clock();

        r0.start();
        r1.start();
        r2.start();
        r3.start();
        r4.start();
        r5.start();

        cout << "\n";

        r0.clock();
        r1.clock();
        r2.clock();
        r3.clock();
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

        r0.in.set(-110);
        r1.in.set(80);
        r2.in.set(120);
        r3.in.set(0);

        cout << "\n";

        r0.clock();
        r1.clock();
        r2.clock();
        r3.clock();
        r4.clock();
        r5.clock();

        r0.start();
        r1.start();
        r2.start();
        r3.start();
        r4.start();
        r5.start();

        cout << "\n";

        r0.clock();
        r1.clock();
        r2.clock();
        r3.clock();
        r4.clock();
        r5.clock();

        CHECK( r5.get_value() == 200 );
        CHECK( r4.get_value() == -30 );

        r0.in.reset();
        r1.in.reset();
        r2.in.reset();
        r3.in.reset();
    }
}
