#ifndef TYPES_H_
#define TYPES_H_

template <int N>
using bits =
    typename std::conditional<N < 8, std::uint8_t,
        typename std::conditional<N < 16, std::uint16_t,
            typename std::conditional<N < 32, std::uint32_t,
                std::uint64_t
            >::type
        >::type
    >::type;
#endif  // TYPES_H_

