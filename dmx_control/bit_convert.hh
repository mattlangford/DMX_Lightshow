#include <cstddef>
#include <array>

// TODO: This seems to get completely optimized out for certain BITS count, figure out why
namespace bit_convert
{

constexpr size_t BYTE_SIZE = 8;

template <size_t BITS, size_t THIS_BIT>
struct convert_helpers
{
    static constexpr size_t BYTES = BITS / BYTE_SIZE;
    static constexpr size_t THIS_BYTE = THIS_BIT / BYTE_SIZE;
    inline static constexpr void convert(std::array<uint8_t, BYTES>& bytes, const std::array<bool, BITS>& bits)
    {
        constexpr size_t SHIFT_AMOUNT = BYTE_SIZE - (THIS_BIT % BYTE_SIZE) - 1;
        bytes[THIS_BYTE] += bits.at(THIS_BIT) << SHIFT_AMOUNT;
        convert_helpers<BITS, THIS_BIT - 1>::convert(bytes, bits);
    }

};

template <size_t BITS>
struct convert_helpers<BITS, 0>
{
    static constexpr size_t BYTES = BITS / BYTE_SIZE;
    inline static constexpr void convert(std::array<uint8_t, BYTES>& bytes, const std::array<bool, BITS>& bits)
    {
        constexpr size_t SHIFT_AMOUNT = BYTE_SIZE - 1;
        bytes[0] += bits.at(0) << SHIFT_AMOUNT;
    }
};

// Convert an array of bools (where each bool will be treated as a bit), into a array of uint8's. Index 0 of the
// bool_array will be the MSb of index 0 of the return array
// This one need > -O1 to get fully optimized out (most of the time, sometimes it stays around)
template <size_t BITS>
std::array<uint8_t, BITS / BYTE_SIZE> convert(const std::array<bool, BITS>& bits)
{
    std::array<uint8_t, BITS / BYTE_SIZE> res;
    // MAJOR TODO: With BITS == 5661 (which is what it will be), this tries to unfold the giant recursion, meaning huge libs
    //convert_helpers<BITS, BITS - 1>::convert(res, bits);
    return res;
}
}
