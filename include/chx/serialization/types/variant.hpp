#pragma once

#include <variant>

#include "./fundamental.hpp"

namespace chx::serialization {
namespace details {
template <std::size_t _I, typename _Fn, typename _V>
decltype(auto) __visit_impl(std::size_t _idx, _Fn&& _f, _V& _v) {
    if (_idx == _I) {
        return _f(std::get<_I>(_v));
    } else {
        if constexpr (_I < std::variant_size_v<_V> - 1) {
            return __visit_impl<_I + 1>(_idx, std::forward<_Fn>(_f), _v);
        }
    }
    std::terminate();
}
template <typename _Fn, typename _V> decltype(auto) __visit(_Fn&& _f, _V& _v) {
    return __visit_impl<0>(_v.index(), std::forward<_Fn>(_f), _v);
}

template <typename R, typename T, typename _Iter, typename _V>
bool __variant_parse_fn(_Iter& ite, _V& v, bool& r, std::size_t& s) {
    std::tuple<T> elem;
    R().parse(ite, elem, r, s);
    if (r) {
        v = std::move_if_noexcept(std::get<0>(elem));
    }
    return r;
}

template <typename, typename> struct __variant_parse;

template <typename... _Rs, typename... _Ts>
struct __variant_parse<std::tuple<_Rs...>, std::tuple<_Ts...>> {
    template <typename _Ite, typename _V>
    void operator()(_Ite& ite, _V& v, bool& r, std::size_t& s) const {
        (__variant_parse_fn<_Rs, _Ts>(ite, v, r, s) || ...);
    }
};
}  // namespace details

template <typename... Types> class rule_t<std::variant<Types...>> {
    using __var_type = std::variant<Types...>;

  public:
    std::size_t generate_calculate(const __var_type& v) const {
        constexpr auto __wrapper = [](const auto& i) {
            using _Tp = std::tuple<decltype(i)>;
            return auto_rule_t<_Tp>().generate_calculate(_Tp(i));
        };
        return details::__visit(__wrapper, v);
    }
    template <typename Iter>
    Iter generate(Iter ite, const __var_type& v) const {
        auto __wrapper = [&ite](const auto& i) {
            using _Tp = std::tuple<decltype(i)>;
            return auto_rule_t<_Tp>().generate(ite, _Tp(i));
        };
        return details::__visit(__wrapper, v);
    }
    template <typename Iter>
    void parse(Iter& ite, __var_type& ctx, bool& r, std::size_t& s) const {
        details::__variant_parse<std::tuple<auto_rule_t<std::tuple<Types>>...>,
                                 std::tuple<Types...>>()(ite, ctx, r, s);
    }
};

template <typename Ori, typename R, typename Ext, typename STp>
class rule_t<alternative<Ori, R, Ext, STp, attrs::VariantReplace>> {
    using __alter = alternative<Ori, R, Ext, STp, attrs::VariantReplace>;

  public:
    std::size_t generate_calculate(typename __alter::const_value_type v) const {
        constexpr auto __wrapper = [](const auto& i) -> std::size_t {
            if constexpr (std::is_same_v<STp, unused_type> &&
                          !std::is_same_v<R, unused_type>) {
                return R().generate_calculate(details::__make_ref_tp(i));
            } else if constexpr (std::is_same_v<R, unused_type>) {
                auto tp = STp()(i);
                return auto_rule_t<decltype(tp)>().generate_calculate(tp);
            } else {
                auto tp = STp()(i);
                return R().generate_calculate(tp);
            }
        };

        return details::__visit(__wrapper, v);
    }
    template <typename Iter>
    Iter generate(Iter ite, typename __alter::const_value_type v) const {
        auto __wrapper = [&ite](const auto& i) {
            if constexpr (std::is_same_v<STp, unused_type> &&
                          !std::is_same_v<R, unused_type>) {
                return R().generate(ite, details::__make_ref_tp(i));
            } else if constexpr (std::is_same_v<R, unused_type>) {
                auto __ref_tp = STp()(i);
                return auto_rule_t<decltype(__ref_tp)>().generate(ite,
                                                                  __ref_tp);
            } else {
                auto __ref_tp = STp()(i);
                return R().generate(ite, __ref_tp);
            }
        };
        return details::__visit(__wrapper, v);
    }

    // TBD
    template <typename Iter>
    void parse(Iter& ite, typename __alter::value_type v, bool& r,
               std::size_t& s) const {}
};
}  // namespace chx::serialization