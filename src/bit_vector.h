#ifndef BITVECTOR_H_
#define BITVECTOR_H_

#include <iostream>
#include <iomanip>

template <int N>
using T =
    typename std::conditional<N < 8, std::uint8_t,
        typename std::conditional<N < 16, std::uint16_t,
            typename std::conditional<N < 32, std::uint32_t,
                std::uint64_t
            >::type
        >::type
    >::type;

template<int N>
T<N> bitmask(T<N> value) {
    T<N> ones = static_cast<T<N>>(~0);
    int shift;
    if (N <= 8) {
        shift = 8 - N;
    } else if (N <= 16) {
        shift = 16 - N;
    } else if (N <= 64) {
        shift = 64 - N;
    }
    return value & (ones >> shift);
}

template <int N>
class BitVector {
public:
    const int length{N};
    BitVector(): value{0} {};
    BitVector(T<N> value): value{bitmask<N>(value)} {};
    BitVector(BitVector<10> const &other): value{other.get_value()} {};

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
        return BitVector<1>{static_cast<T<1>>(value >> i)};
    }

    template <int n, int m>
    BitVector<n - m + 1> slice() {
        T<n - m + 1> tmp = static_cast<T<n-m+1>>(value >> m);
        return BitVector<n-m+1>{bitmask<n - m + 1>(tmp)};
    }

    BitVector signextend(int new_N) {
        if (new_N > N) {
        } else {
        }
    }

    BitVector extend(int new_N) {
        if (new_N > N) {
        } else {
        }
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

template <int N, int M>
BitVector<N + M> concatenate(BitVector<N> const &vec0, BitVector<M> const &vec1) {
    T<N + M> val0 = static_cast<T<N + M>>(vec0.get_value());
    T<N + M> val1 = static_cast<T<N + M>>(vec1.get_value());
    T<N + M> concat_val = (val0 << M) | val1;
    return BitVector<N + M>{concat_val};
}

#endif  // BITVECTOR_H_