#pragma once

#include <cstdint>
#include <stdexcept>
#include <tuple>
/*
types of chx.serialization

C++ TYPE        |   SIZE
std::uint32_t   |   variable size
std::uint64_t   |   variable size
std::int32_t    |   variable size
std::int64_t    |   variable size
double          |       8 Bytes
float           |       4 Bytes
std::string     |   variable size
bool            |       1 Byte

variant
list
optional
user-defined
*/

namespace chx::serialization {
static_assert(sizeof(double) == 8, "size of double is not 8");
static_assert(sizeof(float) == 4, "size of float is not 4");

enum type_flag : unsigned char {
    uint32_flag = 0b0001 << 4,
    uint64_flag = 0b0010 << 4,
    int32_flag = 0b0011 << 4,
    int64_flag = 0b0100 << 4,
    double_flag = 0b0101 << 4,
    float_flag = 0b0110 << 4,
    string_flag = 0b0111 << 4,
    bool_flag = 0b1000 << 4,

    mask = 0b1111 << 4
};

template <typename T> struct is_fundamental : std::false_type {};
template <> struct is_fundamental<std::uint32_t> : std::true_type {};
template <> struct is_fundamental<std::uint64_t> : std::true_type {};
template <> struct is_fundamental<std::int32_t> : std::true_type {};
template <> struct is_fundamental<std::int64_t> : std::true_type {};
template <> struct is_fundamental<double> : std::true_type {};
template <> struct is_fundamental<float> : std::true_type {};
template <typename CharT, typename Traits, typename Allocator>
struct is_fundamental<std::basic_string<CharT, Traits, Allocator>>
    : std::true_type {};
template <> struct is_fundamental<bool> : std::true_type {};

using unused_type = decltype(std::ignore);
constexpr unused_type unused;
}  // namespace chx::serialization