#ifndef BITVECTOR_H_
#define BITVECTOR_H_

#include <iostream>
#include <iomanip>

template <int N>
using T =
    typename std::conditional<N <= 8, std::uint8_t,
        typename std::conditional<N <= 16, std::uint16_t,
            typename std::conditional<N <= 32, std::uint32_t,
                std::uint64_t
            >::type
        >::type
    >::type;

template<int N>
T<N> bitmask(T<N> value) {
    T<N> ones = ~0;
    int shift;
    if (N <= 8) {
        shift = 8 - N;
    } else if (N <= 16) {
        shift = 16 - N;
    } else if (N <= 32) {
        shift = 32 - N;
    } else if (N <= 64) {
        shift = 64 - N;
    }
    return value & (ones >> shift);
}

template<int N>
T<N> bitmask() {
    T<N> ones = ~0;
    int shift;
    if (N <= 8) {
        shift = 8 - N;
    } else if (N <= 16) {
        shift = 16 - N;
    } else if (N <= 32) {
        shift = 32 - N;
    } else if (N <= 64) {
        shift = 64 - N;
    }
    return (ones >> shift);
}

template <int N>
class BitVector {
public:
    const int length{N};
    BitVector(): value{0} {};
    BitVector(T<N> value): value{bitmask<N>(value)} {};
    BitVector(BitVector<N> const &other): value{other.get_value()} {};
    BitVector(BitVector<N> &&other): value{other.get_value()} {};

    // Operators
    bool operator==(BitVector<N> const &other) const {
        return (value == other.value);
    }
    bool operator==(T<N> other) const {
        return (value == other);
    }
    bool operator!=(BitVector<N> const &other) const {
        return (value != other.value);
    }
    BitVector<N>& operator=(BitVector<N> const &other) {
        value = other.value;
        return *this;
    }
    BitVector<N>& operator=(T<N> const &v) {
        value = bitmask<N>(v);
        return *this;
    }
    BitVector<1> const operator[](size_t i) const {
        if (i < N) {
            return BitVector<1>{static_cast<T<1>>(value >> i)};
        } else {
            throw std::out_of_range(
                    "BitVector<" + std::to_string(N) +
                    "> subscript out of range: [" +
                    std::to_string(N - 1) + ", 0]");
        }
    }

    template <int n, int m>
    BitVector<n - m + 1> slice() const {
        T<n - m + 1> tmp = static_cast<T<n-m+1>>(value >> m);
        return BitVector<n-m+1>{bitmask<n - m + 1>(tmp)};
    }

    template <int M>
    BitVector<M> extend() const {
        return BitVector<M>{static_cast<T<M>>(value)};
    }

    template <int M>
    BitVector<M> signextend() const {
        if ((*this)[N - 1] == 1) {
            // msb is 1, extend with ones
            T<M> M_mask = bitmask<M>();
            T<N> N_mask = bitmask<N>();
            T<M> high_mask = M_mask ^ static_cast<T<M>>(N_mask);
            T<M> extended_value = static_cast<T<M>>(value) | high_mask;
            return BitVector<M>{extended_value};
        } else {
            // msb is 0, extend with zeros
            return BitVector<M>{static_cast<T<M>>(value)};
        }
    }

    // Bit mainipulation
    BitVector<N> operator|(BitVector<N> const &other) const {
        T<N> result = value | other.get_value();
        // Result is automatically cut to the correct bitlength
        return BitVector<N>{result};
    }
    BitVector<N> operator&(BitVector<N> const &other) const {
        T<N> result = value & other.get_value();
        // Result is automatically cut to the correct bitlength
        return BitVector<N>{result};
    }
    BitVector<N> operator^(BitVector<N> const &other) const {
        T<N> result = value ^ other.get_value();
        // Result is automatically cut to the correct bitlength
        return BitVector<N>{result};
    }
    BitVector<N> operator~() const {
        T<N> neg = ~value;
        // The value is automatically cut to the correct number of bits in the
        // constructor
        return BitVector<N>{neg};
    }

    // Arithmetics
    BitVector<N> add(BitVector<N> const &other) {
        T<N> sum = get_value() + other.get_value();
        return BitVector<N>{sum};
    }
    BitVector<N> add(BitVector<N> const &other, BitVector<1> const &Cin) {
        T<N> sum = get_value() + other.get_value() + Cin.get_value();
        return BitVector<N>{sum};
    }
    BitVector<N + 1> addc(BitVector<N> const &other) {
        BitVector<N + 1> ext = extend<N+1>();
        BitVector<N + 1> ext_other = other.extend<N+1>();
        T<N + 1> sum = ext.get_value() + ext_other.get_value();
        return BitVector<N + 1>{sum};
    }
    BitVector<N + 1> addc(BitVector<N> const &other, BitVector<1> const &Cin) {
        BitVector<N + 1> ext = extend<N+1>();
        BitVector<N + 1> ext_other = other.extend<N+1>();
        BitVector<N + 1> ext_cin = Cin.extend<N+1>();
        T<N + 1> sum = ext.get_value() + ext_other.get_value() + ext_cin.get_value();
        return BitVector<N + 1>{sum};
    }
    BitVector<N> neg() const {
        T<N> inv = ~value;
        T<N> neg = inv + 1;
        return BitVector<N>{neg};
    }

    T<N> get_value() const {return value;}

private:
    T<N> value;
};

template <int N>
std::ostream& operator<<(std::ostream &os, BitVector<N> const &v) {
    os << std::hex << "0x" << unsigned(v.get_value()) << std::dec;
    return os;
}


// Concatinate BitVectors
//
// I could not figure out how to do this recursively, so you can only
// concatinate 2-4 BitVectors at a time

template <int N, int M>
BitVector<N + M> concatenate(BitVector<N> const &vec0, BitVector<M> const &vec1) {
    T<N + M> val0 = static_cast<T<N + M>>(vec0.get_value());
    T<N + M> val1 = static_cast<T<N + M>>(vec1.get_value());
    T<N + M> concat_val = (val0 << M) | val1;
    return BitVector<N + M>{concat_val};
}

template <int N0, int N1, int N2>
BitVector<N0 + N1 + N2> concatenate(
        BitVector<N0> const &vec0,
        BitVector<N1> const &vec1,
        BitVector<N2> const &vec2) {
    return concatenate<N0, N1 + N2>(vec0, concatenate<N1, N2>(vec1, vec2));
}

template <int N0, int N1, int N2, int N3>
BitVector<N0 + N1 + N2 + N3> concatenate(
        BitVector<N0> const &vec0,
        BitVector<N1> const &vec1,
        BitVector<N2> const &vec2,
        BitVector<N3> const &vec3) {
    return concatenate<N0, N1 + N2 + N3>(vec0, concatenate<N1, N2, N3>(vec1, vec2, vec3));
}

#endif  // BITVECTOR_H_
