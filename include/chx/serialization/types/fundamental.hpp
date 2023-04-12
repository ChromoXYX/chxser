#pragma once

#include <bitset>
#include <cstring>

#include "../rule.hpp"

namespace chx::serialization {
template <> class rule_t<std::uint32_t> {
  public:
    std::size_t generate_calculate(std::uint32_t i) const noexcept(true) {
        constexpr static const std::size_t __table[] = {0, 0x100, 0x10000,
                                                        0x1000000};
        if (i == 0) {
            return 1;
        }
        for (std::size_t idx = 1; idx < 4; ++idx) {
            if (i < __table[idx]) {
                return idx + 1;
            }
        }
        return 5;
    }

    template <typename Iter> Iter generate(Iter ite, std::uint32_t i) const {
        unsigned char __buffer[1 + sizeof(i)] = {0};

        std::size_t __size = generate_calculate(i);
        __buffer[0] = uint32_flag | static_cast<unsigned char>(__size);
        std::memcpy(&__buffer[1], &i, __size - 1);
        for (std::size_t j = 0; j < __size; ++j) {
            *(ite++) = __buffer[j];
        }
        return ite;
    }

    template <typename Iter>
    void parse(Iter &ite, std::uint32_t &ctx, bool &r, std::size_t &s) const {
        if (s >= 1) {
            char first = *ite;
            if ((first & mask) != uint32_flag) {
                r = false;
                return;
            }

            first = first & (~mask);
            std::size_t size = 0;
            std::memcpy(&size, &first, sizeof(first));
            if (s < size) {
                r = false;
                return;
            }
            if (--size > sizeof(std::uint32_t)) {
                r = false;
                return;
            }
            s -= size + 1;
            ++ite;

            ctx = 0;
            unsigned char __buffer[sizeof(std::uint32_t)] = {0};
            for (int i = 0; i < size; ++i) {
                __buffer[i] = *(ite++);
            }
            std::memcpy(&ctx, __buffer, sizeof(__buffer));
            r = true;
            return;
        } else {
            r = false;
            return;
        }
    }
};

template <> class rule_t<std::uint64_t> {
  public:
    std::size_t generate_calculate(std::uint64_t i) const noexcept(true) {
        constexpr static const std::size_t __table[] = {
            0,           0x100,         0x10000,         0x1000000,
            0x100000000, 0x10000000000, 0x1000000000000, 0x100000000000000};
        if (i == 0) {
            return 1;
        }
        for (std::size_t idx = 1; idx < 8; ++idx) {
            if (i < __table[idx]) {
                return idx + 1;
            }
        }
        return 9;
    }
    template <typename Iter> Iter generate(Iter ite, std::uint64_t i) const {
        unsigned char __buffer[1 + sizeof(i)] = {0};

        std::size_t __size = generate_calculate(i);
        __buffer[0] = uint64_flag | static_cast<unsigned char>(__size);
        std::memcpy(&__buffer[1], &i, __size - 1);
        for (std::size_t j = 0; j < __size; ++j) {
            *(ite++) = __buffer[j];
        }
        return ite;
    }

    template <typename Iter>
    void parse(Iter &ite, std::uint64_t &ctx, bool &r, std::size_t &s) const {
        if (s >= 1) {
            char first = *ite;
            if ((first & mask) != uint64_flag) {
                r = false;
                return;
            }

            first = first & (~mask);
            std::size_t size = 0;
            std::memcpy(&size, &first, sizeof(first));
            if (s < size) {
                r = false;
                return;
            }
            if (--size > sizeof(std::uint64_t)) {
                r = false;
                return;
            }
            s -= size + 1;
            ++ite;

            ctx = 0;
            unsigned char __buffer[sizeof(std::uint64_t)] = {0};
            for (int i = 0; i < size; ++i) {
                __buffer[i] = *(ite++);
            }
            std::memcpy(&ctx, __buffer, sizeof(__buffer));
            r = true;
            return;
        } else {
            r = false;
            return;
        }
    }
};

template <> class rule_t<std::int32_t> {
  public:
    std::size_t generate_calculate(std::int32_t i) const noexcept(true) {
        i = std::abs(i);
        constexpr static const std::size_t __table[] = {0, 0x100, 0x10000,
                                                        0x1000000};
        if (i == 0) {
            return 1;
        }
        for (std::size_t idx = 1; idx < 4; ++idx) {
            if (i < __table[idx]) {
                return idx + 1;
            }
        }
        return 5;
    }
    template <typename Iter> Iter generate(Iter ite, std::int32_t num) const {
        unsigned char __buffer[1 + sizeof(num)] = {0};

        std::size_t __size = generate_calculate(num);
        __buffer[0] = int32_flag | ((num < 0) ? 0b1000 : 0b0000) |
                      static_cast<unsigned char>(__size);
        num = std::abs(num);
        std::memcpy(&__buffer[1], &num, __size - 1);
        for (std::size_t j = 0; j < __size; ++j) {
            *(ite++) = __buffer[j];
        }
        return ite;
    }

    template <typename Iter>
    void parse(Iter &ite, std::int32_t &ctx, bool &r, std::size_t &s) const {
        if (s >= 1) {
            char first = *ite;
            if ((first & mask) != int32_flag) {
                r = false;
                return;
            }

            first = first & (~mask);
            bool is_neg = first & 0b1000;
            first = first & 0b0111;
            std::size_t size = 0;
            std::memcpy(&size, &first, sizeof(first));
            if (s < size) {
                r = false;
                return;
            }
            if (--size > sizeof(std::int32_t)) {
                r = false;
                return;
            }
            s -= size + 1;
            ++ite;

            ctx = 0;
            unsigned char __buf[sizeof(std::int32_t)] = {0};
            for (int i = 0; i < size; ++i) {
                __buf[i] = *(ite++);
            }
            std::memcpy(&ctx, __buf, sizeof(__buf));
            ctx = is_neg ? -ctx : ctx;
            r = true;
            return;
        } else {
            r = false;
            return;
        }
    }
};

template <> class rule_t<std::int64_t> {
  public:
    std::size_t generate_calculate(std::int64_t i) const noexcept(true) {
        i = std::abs(i);
        constexpr static const std::size_t __table[] = {
            0,           0x100,         0x10000,         0x1000000,
            0x100000000, 0x10000000000, 0x1000000000000, 0x100000000000000};
        for (std::size_t idx = 1; idx < 8; ++idx) {
            if (i < __table[idx]) {
                return idx + 1;
            }
        }
        return 9;
    }
    template <typename Iter> Iter generate(Iter ite, std::int64_t num) const {
        unsigned char __buffer[1 + sizeof(num)] = {0};

        std::size_t __size = generate_calculate(num);
        __buffer[0] = int64_flag | ((num < 0) ? 0b1000 : 0b0000) |
                      static_cast<unsigned char>(__size);
        num = std::abs(num);
        std::memcpy(&__buffer[1], &num, __size - 1);
        for (std::size_t j = 0; j < __size; ++j) {
            *(ite++) = __buffer[j];
        }
        return ite;
    }

    template <typename Iter>
    void parse(Iter &ite, std::int64_t &ctx, bool &r, std::size_t &s) const {
        if (s >= 2) {
            char first = *ite;
            if ((first & mask) != int64_flag) {
                r = false;
                return;
            }

            first = first & (~mask);
            bool is_neg = first & 0b1000;
            first = first & 0b0111;
            std::size_t size = 0;
            std::memcpy(&size, &first, sizeof(first));
            if (s < size) {
                r = false;
                return;
            }
            if (--size > sizeof(std::int64_t)) {
                r = false;
                return;
            }
            s -= size + 1;
            ++ite;

            ctx = 0;
            unsigned char __buf[sizeof(std::int64_t)] = {0};
            for (int i = 0; i < size; ++i) {
                __buf[i] = *(ite++);
            }
            std::memcpy(&ctx, __buf, sizeof(__buf));
            ctx = is_neg ? -ctx : ctx;
            r = true;
            return;
        } else {
            r = false;
            return;
        }
    }
};

template <> class rule_t<double> {
  public:
    constexpr std::size_t generate_calculate(double) const noexcept(true) {
        return sizeof(double) + 1;
    }
    template <typename Iter> Iter generate(Iter ite, double i) const {
        unsigned char __buffer[1 + sizeof(i)] = {0};
        __buffer[0] = double_flag;
        std::memcpy(&__buffer[1], &i, sizeof(double));
        for (auto &j : __buffer) {
            *(ite++) = j;
        }
        return ite;
    }

    template <typename Iter>
    void parse(Iter &ite, double &ctx, bool &r, std::size_t &s) const {
        if (s >= sizeof(double) + 1) {
            char first = *ite;
            if ((first & mask) != double_flag) {
                r = false;
                return;
            }
            s -= sizeof(double) + 1;
            ++ite;

            ctx = 0;
            unsigned char __buf[sizeof(double)] = {0};
            for (int i = 0; i < sizeof(double); ++i) {
                __buf[i] = *(ite++);
            }
            std::memcpy(&ctx, __buf, sizeof(__buf));
            r = true;
            return;
        } else {
            r = false;
            return;
        }
    }
};

template <> class rule_t<float> {
  public:
    constexpr std::size_t generate_calculate(float) const noexcept(true) {
        return sizeof(float) + 1;
    }
    template <typename Iter> Iter generate(Iter ite, float i) const {
        unsigned char __buffer[1 + sizeof(i)] = {0};
        __buffer[0] = float_flag;
        std::memcpy(&__buffer[1], &i, sizeof(float));
        for (auto &j : __buffer) {
            *(ite++) = j;
        }
        return ite;
    }

    template <typename Iter>
    void parse(Iter &ite, float &ctx, bool &r, std::size_t &s) const {
        if (s >= sizeof(float) + 1) {
            char first = *ite;
            if ((first & mask) != double_flag) {
                r = false;
                return;
            }
            s -= sizeof(float) + 1;
            ++ite;

            ctx = 0;
            unsigned char __buf[sizeof(float)] = {0};
            for (int i = 0; i < sizeof(float); ++i) {
                __buf[i] = *(ite++);
            }
            std::memcpy(&ctx, __buf, sizeof(__buf));
            r = true;
            return;
        } else {
            r = false;
            return;
        }
    }
};

template <typename CharT, typename Traits, typename Allocator>
class rule_t<std::basic_string<CharT, Traits, Allocator>> {
    using _val_type = std::basic_string<CharT, Traits, Allocator>;

  public:
    std::size_t generate_calculate(const _val_type &str) const noexcept(true) {
        return rule_t<std::uint64_t>().generate_calculate(str.size()) +
               str.size();
    }
    template <typename Iter>
    Iter generate(Iter ite, const _val_type &str) const {
        std::size_t str_size = str.size();
        unsigned char __head[1 + sizeof(std::uint64_t)] = {0};
        std::size_t __head_size =
            rule_t<std::uint64_t>().generate_calculate(str_size);
        __head[0] = string_flag | static_cast<unsigned char>(__head_size);
        std::memcpy(&__head[1], &str_size, __head_size - 1);
        for (std::size_t j = 0; j < __head_size; ++j) {
            *(ite++) = __head[j];
        }
        for (auto &i : str) {
            *(ite++) = i;
        }
        return ite;
    }

    template <typename Iter, typename _CharT, typename _Tratis, typename _Alloc>
    void parse(Iter &ite, std::basic_string<_CharT, _Tratis, _Alloc> &ctx,
               bool &r, std::size_t &s) const {
        if (s >= 1) {
            auto ite_back = ite;
            auto s_back = s;

            char first = *ite;
            if ((first & mask) != string_flag) {
                r = false;
                return;
            }

            first = first & (~mask);
            std::size_t size = 0;
            std::memcpy(&size, &first, sizeof(first));
            if (s < size) {
                r = false;
                return;
            }
            if (--size > sizeof(std::uint64_t)) {
                r = false;
                return;
            }
            s -= size + 1;
            ++ite;

            std::size_t str_size = 0;
            unsigned char __str_size_buf[sizeof(std::uint64_t)] = {0};
            for (int i = 0; i < size; ++i) {
                __str_size_buf[i] = *(ite++);
            }
            std::memcpy(&str_size, __str_size_buf, sizeof(__str_size_buf));
            if (s < str_size) {
                r = false;
                s = s_back;
                ite = ite_back;
                return;
            }
            auto end = ite;
            std::advance(end, str_size);
            ctx.assign(ite, end);

            ite = end;
            s -= str_size;
            r = true;
            return;
        } else {
            r = false;
            return;
        }
    }
};

template <> class rule_t<bool> {
  public:
    constexpr std::size_t generate_calculate(bool) const noexcept(true) {
        return 1;
    }
    template <typename Iter> Iter generate(Iter ite, bool b) const {
        *(ite++) = bool_flag | (b ? 0b1 : 0b0);
        return ite;
    }

    template <typename Iter>
    void parse(Iter &ite, bool &ctx, bool &r, std::size_t &s) const {
        if (s >= 1) {
            char first = *ite;
            if ((first & mask) != bool_flag) {
                r = false;
                return;
            }
            first = first & (~mask);
            switch (first) {
                case 0b1: {
                    ctx = true;
                    break;
                }
                case 0b0: {
                    ctx = false;
                    break;
                }
                default: {
                    r = false;
                    return;
                }
            }
            --s;
            ++ite;
            r = true;
            return;
        } else {
            r = false;
            return;
        }
    }
};

//
constexpr inline rule_t<rule_t<std::uint32_t>> uint32_;
constexpr inline rule_t<rule_t<std::uint64_t>> uint64_;
constexpr inline rule_t<rule_t<std::int32_t>> int32_;
constexpr inline rule_t<rule_t<std::int64_t>> int64_;
constexpr inline rule_t<rule_t<float>> float_;
constexpr inline rule_t<rule_t<double>> double_;
constexpr inline rule_t<rule_t<std::string>> str_;
constexpr inline rule_t<rule_t<bool>> bool_;
}  // namespace chx::serialization