#pragma once

#include <tuple>

#include "rule.hpp"

namespace chx::serialization {
namespace details {
template <typename T>
constexpr typename std::add_rvalue_reference_t<T> __declval() noexcept(true) {}
}  // namespace details

// all member type `type' is NOT actually return type of operator()

template <typename... Wrappers> struct struct_tuple;

template <typename W> struct struct_tuple<W> {
    using type = typename W::type;

    template <typename C>
    constexpr auto operator()(C& t) const noexcept(noexcept(W()(t))) {
        return W()(t);
    }
};
template <typename W, typename... Ts> struct struct_tuple<W, Ts...> {
    using type = decltype(std::tuple_cat(
        std::declval<typename W::type>(),
        std::declval<typename struct_tuple<Ts...>::type>()));

    template <typename C>
    constexpr auto operator()(C& t) const
        noexcept(noexcept(W()(t)) && noexcept(struct_tuple<Ts...>()(t))) {
        return std::tuple_cat(W()(t), struct_tuple<Ts...>()(t));
    }
};

template <typename... L, typename... R>
constexpr struct_tuple<L..., R...> operator<<(
    struct_tuple<L...>, struct_tuple<R...>) noexcept(true) {
    return {};
}

template <auto Ptr> struct as {
    using type = std::tuple<std::add_lvalue_reference_t<
        typename details::__get_type_from_memptr<decltype(Ptr)>::type>>;

    template <typename T> constexpr auto operator()(T& t) const noexcept(true) {
        if constexpr (std::is_const_v<T>) {
            return std::tuple<std::add_lvalue_reference_t<
                std::add_const_t<typename details::__get_type_from_memptr<
                    decltype(Ptr)>::type>>>{t.*Ptr};
        } else {
            return type{t.*Ptr};
        }
    }
};
template <auto Ptr, typename W> struct as_expand {
    using type = typename W::type;

    template <typename T>
    constexpr auto operator()(T& t) const noexcept(noexcept(W()(t.*Ptr))) {
        return W()(t.*Ptr);
    }
};
template <auto Ptr, typename W> struct as_tuple {
    using type = std::tuple<typename W::type>;

    template <typename T>
    constexpr auto operator()(T& t) const noexcept(noexcept(W()(t.*Ptr))) {
        if constexpr (std::is_const_v<T>) {
            return std::tuple<const typename W::type>{W()(t.*Ptr)};
        } else {
            return type{W()(t.*Ptr)};
        }
    }
};
template <typename O, typename W, typename Extracter,
          typename STp = unused_type, typename Attr = unused_type>
struct as_alter {
    using type = std::tuple<alternative<O, W, Extracter, STp, Attr>>;

    template <typename T>
    constexpr auto operator()(T&& t) const noexcept(true) {
        if constexpr (!std::is_const_v<std::remove_reference_t<T>>) {
            return type{t};
        } else {
            if constexpr (std::is_lvalue_reference_v<T>) {
                return std::tuple<
                    alternative<std::add_lvalue_reference_t<
                                    const std::remove_reference_t<O>>,
                                W, Extracter, STp, Attr>>{t};
            } else {
                return std::tuple<
                    alternative<const O, W, Extracter, STp, Attr>>{t};
            }
        }
    }
};

template <typename... Tps> constexpr decltype(auto) merge(Tps&&... tps) {
    return std::tuple_cat(tps...);
}

template <auto... Ps, typename T>
constexpr auto auto_make_tp(T& t) noexcept(true) {
    return struct_tuple<as<Ps>...>()(t);
};
template <typename T, auto... Ps>
using auto_tuple_t = decltype(auto_make_tp<Ps...>(
    details::__declval<std::add_lvalue_reference_t<T>>()));
}  // namespace chx::serialization