#pragma once

#include <forward_list>
#include <list>
#include <vector>

#include "./fundamental.hpp"

namespace chx::serialization {
template <typename _C> struct list_base {
    std::size_t generate_calculate(const _C& _c) const {
        std::size_t __ssize = 0;

        using _Tp = std::tuple<std::add_lvalue_reference_t<
            std::add_const_t<typename _C::value_type>>>;
        for (auto& _i : _c) {
            __ssize += auto_rule_t<_Tp>().generate_calculate(_Tp{_i});
        }
        return __ssize;
    }
    template <typename Iter> Iter generate(Iter ite, const _C& _c) const {
        if constexpr (std::is_same_v<typename _C::value_type, bool>) {
            using _Tp = std::tuple<bool>;
            for (auto _i : _c) {
                ite = auto_rule_t<_Tp>().generate(ite, _Tp{_i});
            }
            return ite;
        } else {
            using _Tp = std::tuple<std::add_lvalue_reference_t<
                std::add_const_t<typename _C::value_type>>>;
            for (auto& _i : _c) {
                ite = auto_rule_t<_Tp>().generate(ite, _Tp{_i});
            }
            return ite;
        }
    }
    template <typename Iter>
    void parse(Iter& ite, _C& ctx, bool& r, std::size_t& s) const {
        using _vt = typename _C::value_type;
        std::tuple<_vt> elem;
        auto_rule_t<decltype(elem)> elem_rule;
        for (elem_rule.parse(ite, elem, r, s); r;
             elem_rule.parse(ite, elem, r, s)) {
            ctx.emplace_back(std::move_if_noexcept(std::get<0>(elem)));
        }
        r = true;
    }
};

template <typename T, typename Allocator>
class rule_t<std::vector<T, Allocator>>
    : public list_base<std::vector<T, Allocator>> {};

template <typename T, typename Allocator>
class rule_t<std::list<T, Allocator>>
    : public list_base<std::vector<T, Allocator>> {};

template <typename T, typename Allocator>
class rule_t<std::forward_list<T, Allocator>>
    : public list_base<std::vector<T, Allocator>> {};

template <typename Ori, typename R, typename Ext, typename STp>
class rule_t<alternative<Ori, R, Ext, STp, attrs::ListReplace>> {
    using __alter = alternative<Ori, R, Ext, STp, attrs::ListReplace>;
    using __vt =
        typename std::decay_t<typename __alter::value_type>::value_type;

  public:
    std::size_t generate_calculate(typename __alter::const_value_type v) const {
        if (!v.empty()) {
            std::size_t sz = 0;
            for (auto& i : v) {
                if constexpr (std::is_same_v<STp, unused_type> &&
                              !std::is_same_v<R, unused_type>) {
                    sz += R().generate_calculate(details::__make_ref_tp(i));
                } else if constexpr (std::is_same_v<R, unused_type>) {
                    auto __ref_tp = STp()(i);
                    sz += auto_rule_t<decltype(__ref_tp)>().generate_calculate(
                        __ref_tp);
                } else {
                    auto __ref_tp = STp()(i);
                    sz += R().generate_calculate(__ref_tp);
                }
            }
            return sz;
        } else {
            return 0;
        }
    }
    template <typename Iter>
    Iter generate(Iter ite, typename __alter::const_value_type v) const {
        for (auto& i : v) {
            if constexpr (std::is_same_v<STp, unused_type> &&
                          !std::is_same_v<R, unused_type>) {
                ite = R().generate(ite, details::__make_ref_tp(i));
            } else if constexpr (std::is_same_v<R, unused_type>) {
                auto __ref_tp = STp()(i);
                ite = auto_rule_t<decltype(__ref_tp)>().generate(ite, __ref_tp);
            } else {
                auto __ref_tp = STp()(i);
                ite = R().generate(ite, __ref_tp);
            }
        }
        return ite;
    }
    template <typename Iter>
    void parse(Iter& ite, typename __alter::value_type v, bool& r,
               std::size_t& s) const {
        __vt elem;
        if constexpr (std::is_same_v<STp, unused_type> &&
                      !std::is_same_v<R, unused_type>) {
            auto __ref_tp = details::__make_ref_tp(elem);
            for (R().parse(ite, __ref_tp, r, s); r;
                 R().parse(ite, __ref_tp, r, s)) {
                v.emplace_back(std::move_if_noexcept(elem));
            }
        } else if constexpr (std::is_same_v<R, unused_type>) {
            auto __ref_tp = STp()(elem);
            auto ru = auto_rule_t<decltype(__ref_tp)>();
            for (ru.parse(ite, __ref_tp, r, s); r;
                 ru.parse(ite, __ref_tp, r, s)) {
                v.emplace_back(std::move_if_noexcept(elem));
            }
        } else {
            auto __ref_tp = STp()(elem);
            for (R().parse(ite, __ref_tp, r, s); r;
                 R().parse(ite, __ref_tp, r, s)) {
                v.emplace_back(std::move_if_noexcept(elem));
            }
        }
        r = true;
    }
};
}  // namespace chx::serialization