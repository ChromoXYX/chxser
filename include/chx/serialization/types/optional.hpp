#pragma once

#include <optional>

#include "./fundamental.hpp"

namespace chx::serialization {
template <typename T> class rule_t<std::optional<T>> {
  public:
    std::size_t generate_calculate(const std::optional<T>& opt) const {
        if (opt.has_value()) {
            std::tuple<const T&> __tp{opt.value()};
            return auto_rule_t<std::tuple<const T&>>().generate_calculate(__tp);
        } else {
            return 0;
        }
    }
    template <typename Iter>
    Iter generate(Iter ite, const std::optional<T>& opt) const {
        if (opt.has_value()) {
            std::tuple<const T&> __tp{opt.value()};
            return auto_rule_t<std::tuple<const T&>>().generate(ite, __tp);
        } else {
            return ite;
        }
    }
    template <typename Iter>
    void parse(Iter& ite, std::optional<T>& ctx, bool& r,
               std::size_t& s) const {
        std::tuple<T> elem;
        auto_rule_t<decltype(elem)> elem_rule;
        elem_rule.parse(ite, elem, r, s);
        if (r) {
            ctx.emplace(std::move_if_noexcept(std::get<0>(elem)));
        } else {
            r = true;
            ctx = std::nullopt;
        }
    }
};
template <typename Ori, typename R, typename Ext, typename STp>
class rule_t<alternative<Ori, R, Ext, STp, attrs::OptionalReplace>> {
    using __alter = alternative<Ori, R, Ext, STp, attrs::OptionalReplace>;
    using __vt =
        typename std::decay_t<typename __alter::value_type>::value_type;

  public:
    std::size_t generate_calculate(typename __alter::const_value_type v) const {
        if (v.has_value()) {
            if constexpr (std::is_same_v<STp, unused_type> &&
                          !std::is_same_v<R, unused_type>) {
                return R().generate_calculate(
                    details::__make_ref_tp(v.value()));
            } else if constexpr (std::is_same_v<R, unused_type>) {
                auto __ref_tp = STp()(v.value());
                return auto_rule_t<decltype(__ref_tp)>().generate_calculate(
                    __ref_tp);
            } else {
                auto __ref_tp = STp()(v.value());
                return R().generate_calculate(__ref_tp);
            }
        } else {
            return 0;
        }
    }
    template <typename Iter>
    Iter generate(Iter ite, typename __alter::const_value_type v) const {
        if (v.has_value()) {
            if constexpr (std::is_same_v<STp, unused_type> &&
                          !std::is_same_v<R, unused_type>) {
                return R().generate(ite, details::__make_ref_tp(v.value()));
            } else if constexpr (std::is_same_v<R, unused_type>) {
                auto __ref_tp = STp()(v.value());
                return auto_rule_t<decltype(__ref_tp)>().generate(ite,
                                                                  __ref_tp);
            } else {
                auto __ref_tp = STp()(v.value());
                return R().generate(ite, __ref_tp);
            }
        } else {
            return ite;
        }
    }
    template <typename Iter>
    void parse(Iter& ite, typename __alter::value_type v, bool& r,
               std::size_t& s) const {
        typename std::decay_t<decltype(v)>::value_type elem;
        if constexpr (std::is_same_v<STp, unused_type> &&
                      !std::is_same_v<R, unused_type>) {
            auto __ref_tp = details::__make_ref_tp(elem);
            R().parse(ite, __ref_tp, r, s);
        } else if constexpr (std::is_same_v<R, unused_type>) {
            auto __tp = STp()(elem);
            auto_rule_t<decltype(__tp)>().parse(ite, __tp, r, s);
        } else {
            auto __ref_tp = STp()(elem);
            R().parse(ite, __ref_tp, r, s);
        }
        if (r) {
            v.emplace(std::move_if_noexcept(elem));
        } else {
            r = true;
        }
    }
};
}  // namespace chx::serialization