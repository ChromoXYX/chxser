#pragma once

#include "utility.hpp"

namespace chx::serialization {
namespace details {
template <typename... _Ts> struct __args_expand;
template <typename... _Rs, typename... _Ts>
struct __args_expand<std::tuple<_Rs...>, _Ts...> {
    using type = decltype(std::tuple_cat(
        std::declval<typename __args_expand<_Rs...>::type>(),
        std::declval<typename __args_expand<_Ts...>::type>()));
};
template <typename _T, typename... _Ts> struct __args_expand<_T, _Ts...> {
    using type = decltype(std::tuple_cat(
        std::declval<std::tuple<_T>>(),
        std::declval<typename __args_expand<_Ts...>::type>()));
};
template <typename... _Ts> struct __args_expand<std::tuple<_Ts...>> {
    using type = typename __args_expand<_Ts...>::type;
};
template <typename _T> struct __args_expand<_T> {
    using type = std::tuple<_T>;
};

template <typename _Tp> struct __tp_expand;
template <typename... _Ts> struct __tp_expand<std::tuple<_Ts...>> {
    using type = typename __args_expand<_Ts...>::type;
};

template <typename _Tp> using __tuple_expand = __tp_expand<std::decay_t<_Tp>>;

template <typename _T> struct __is_tuple_impl : std::false_type {};
template <typename... _Ts>
struct __is_tuple_impl<std::tuple<_Ts...>> : std::true_type {};
template <typename Tp> using is_tuple = __is_tuple_impl<std::decay_t<Tp>>;

template <std::size_t N, typename... _Ts> struct __split_tuple;
template <std::size_t N, typename _T, typename... _Ts>
struct __split_tuple<N, _T, _Ts...> {
    using prefix_type = decltype(std::tuple_cat(
        std::declval<std::tuple<_T>>(),
        std::declval<typename __split_tuple<N - 1, _Ts...>::prefix_type>()));
    using suffix_type = typename __split_tuple<N - 1, _Ts...>::suffix_type;
};
template <typename _T, typename... _Ts> struct __split_tuple<1, _T, _Ts...> {
    using prefix_type = std::tuple<_T>;
    using suffix_type = std::tuple<_Ts...>;
};

template <typename _Tp, std::size_t _I, std::size_t... _Is>
constexpr decltype(auto) __tp_expand_ref(
    _Tp &tp, std::integer_sequence<std::size_t, _I, _Is...>) {
    constexpr auto const __obj_to_ref_tp = [](auto &i) -> decltype(auto) {
        return std::tuple<std::add_lvalue_reference_t<decltype(i)>>{i};
    };
    if constexpr (is_tuple<std::tuple_element_t<_I, _Tp>>::value) {
        if constexpr (sizeof...(_Is) != 0) {
            return std::tuple_cat(
                __tp_expand_ref(
                    std::get<_I>(tp),
                    std::make_integer_sequence<
                        std::size_t, std::tuple_size_v<std::decay_t<
                                         std::tuple_element_t<_I, _Tp>>>>()),
                __tp_expand_ref(tp,
                                std::integer_sequence<std::size_t, _Is...>()));
        } else {
            return __tp_expand_ref(
                std::get<_I>(tp),
                std::make_integer_sequence<
                    std::size_t, std::tuple_size_v<std::decay_t<
                                     std::tuple_element_t<_I, _Tp>>>>());
        }
    } else {
        if constexpr (sizeof...(_Is) != 0) {
            return std::tuple_cat(
                __obj_to_ref_tp(std::get<_I>(tp)),
                __tp_expand_ref(tp,
                                std::integer_sequence<std::size_t, _Is...>()));
        } else {
            return __obj_to_ref_tp(std::get<_I>(tp));
        }
    }
}
template <typename _Tp, std::size_t _I, std::size_t... _Is>
constexpr decltype(auto) __tp_expand_const_ref(
    _Tp &tp, std::integer_sequence<std::size_t, _I, _Is...>) {
    constexpr auto const __obj_to_ref_tp = [](auto &i) -> decltype(auto) {
        return std::tuple<
            std::add_const_t<std::add_lvalue_reference_t<decltype(i)>>>{i};
    };
    if constexpr (is_tuple<std::tuple_element_t<_I, _Tp>>::value) {
        if constexpr (sizeof...(_Is) != 0) {
            return std::tuple_cat(
                __tp_expand_ref(
                    std::get<_I>(tp),
                    std::make_integer_sequence<
                        std::size_t, std::tuple_size_v<std::decay_t<
                                         std::tuple_element_t<_I, _Tp>>>>()),
                __tp_expand_ref(tp,
                                std::integer_sequence<std::size_t, _Is...>()));
        } else {
            return __tp_expand_ref(
                std::get<_I>(tp),
                std::make_integer_sequence<
                    std::size_t, std::tuple_size_v<std::decay_t<
                                     std::tuple_element_t<_I, _Tp>>>>());
        }
    } else {
        if constexpr (sizeof...(_Is) != 0) {
            return std::tuple_cat(
                __obj_to_ref_tp(std::get<_I>(tp)),
                __tp_expand_ref(tp,
                                std::integer_sequence<std::size_t, _Is...>()));
        } else {
            return __obj_to_ref_tp(std::get<_I>(tp));
        }
    }
}

template <typename _Tp>
constexpr decltype(auto) __flat_tp_ref(_Tp &tp) noexcept(true) {
    return __tp_expand_ref(
        tp, std::make_integer_sequence<std::size_t, std::tuple_size_v<_Tp>>());
}
template <typename _Tp>
constexpr decltype(auto) __flat_tp_const_ref(_Tp &tp) noexcept(true) {
    return __tp_expand_const_ref(
        tp, std::make_integer_sequence<std::size_t, std::tuple_size_v<_Tp>>());
}

template <std::size_t Idx, typename _Tp> struct __reverse_tuple {
    using type = decltype(std::tuple_cat(
        std::declval<std::tuple<std::tuple_element_t<Idx, _Tp>>>(),
        std::declval<typename __reverse_tuple<Idx - 1, _Tp>::type>()));
};
template <typename _Tp> struct __reverse_tuple<0, _Tp> {
    using type = std::tuple<std::tuple_element_t<0, _Tp>>;
};

template <typename P> struct __get_type_from_memptr;
template <typename C, typename P> struct __get_type_from_memptr<P C::*> {
    using type = P;
};

template <typename T> constexpr auto __make_ref_tp(T &t) noexcept(true) {
    return std::tuple<T &>(t);
}
}  // namespace details

template <typename T, typename AlterRule, typename Extracter,
          typename StructTuple = unused_type, typename Attribute = unused_type>
struct alternative {
    using type = alternative;
    using value_type = typename Extracter::return_type;
    using const_value_type = typename Extracter::const_return_type;

    using original_type = T;
    using rule_type = AlterRule;
    using struct_tp = StructTuple;
    using attribute = Attribute;

    template <typename Ori> alternative(Ori &&o) : origin(o) {}

    original_type origin;

    value_type get() { return Extracter()(origin); }
    const_value_type get() const { return Extracter()(origin); }
};
namespace attrs {
struct OptionalReplace {};
struct VariantReplace {};
struct ListReplace {};
}  // namespace attrs
template <auto Ptr> struct default_extractor {
    using return_type =
        typename details::__get_type_from_memptr<decltype(Ptr)>::type &;
    using const_return_type =
        const typename details::__get_type_from_memptr<decltype(Ptr)>::type &;

    template <typename T>
    constexpr decltype(auto) operator()(T &t) const noexcept(true) {
        return t.*Ptr;
    }
};

template <typename T> struct is_alternative : std::false_type {};
template <typename Origin, typename A, typename STp, typename Extracter,
          typename Attribute>
struct is_alternative<alternative<Origin, A, STp, Extracter, Attribute>>
    : std::true_type {};

template <typename... Rules> class rule_t {
    template <typename T> static decltype(auto) __alter_get(T &t) {
        if constexpr (is_alternative<std::decay_t<T>>::value) {
            return t.get();
        } else {
            return t;
        }
    }
    // start stateless generate calculate
    template <typename... _Ts> struct __generator_cal;

    template <std::size_t _Idx, typename _C, typename _T, typename _R>
    struct __generator_cal_merge;
    template <std::size_t _Idx, typename _C, typename... _Ts, typename... _Rs>
    struct __generator_cal_merge<_Idx, _C, std::tuple<_Ts...>,
                                 std::tuple<_Rs...>> {
        template <typename _Tp> std::size_t operator()(_Tp &&_tp) const {
            if constexpr (sizeof...(_Rs) != 0) {
                return __generator_cal<std::tuple_element_t<_Idx, _C>, _Ts...>()
                           .template f<0>(std::get<_Idx>(_tp)) +
                       __generator_cal<_C, _Rs...>().template f<_Idx + 1>(_tp);
            } else {
                return __generator_cal<std::tuple_element_t<_Idx, _C>, _Ts...>()
                    .template f<0>(std::get<_Idx>(_tp));
            }
        }
    };

    template <typename _Tp, typename _T, typename... _Ts>
    struct __generator_cal<_Tp, _T, _Ts...> {
        template <std::size_t _Idx> std::size_t f(_Tp &_tp) const {
            if constexpr (!details::is_tuple<
                              std::tuple_element_t<_Idx, _Tp>>::value) {
                if constexpr (sizeof...(_Ts) != 0) {
                    return _T().generate_calculate(
                               __alter_get(std::get<_Idx>(_tp))) +
                           __generator_cal<_Tp, _Ts...>().template f<_Idx + 1>(
                               _tp);
                } else {
                    return _T().generate_calculate(
                        __alter_get(std::get<_Idx>(_tp)));
                }
            } else {
                if constexpr (sizeof...(_Ts) == 0) {
                    return __generator_cal<std::tuple_element_t<_Idx, _Tp>,
                                           _T>()
                        .template f<0>(std::get<_Idx>(_tp));
                } else {
                    using __split = details::__split_tuple<
                        std::tuple_size_v<typename details::__tuple_expand<
                            std::tuple_element_t<_Idx, _Tp>>::type>,
                        _T, _Ts...>;
                    return __generator_cal_merge<
                        _Idx, _Tp, typename __split::prefix_type,
                        typename __split::suffix_type>()(_tp);
                }
            }
        }
    };

    template <typename _Ctx, typename _Tp> struct __generate_cal_impl;
    template <typename _Ctx, typename... _Ts>
    struct __generate_cal_impl<_Ctx, std::tuple<_Ts...>>
        : __generator_cal<_Ctx, _Ts...> {};
    // end stateless generate calculate

    // begin stateful generate calculate
    template <typename _Ret, typename _Ctx, typename _Tp, std::size_t _Idx,
              typename _Wrapper>
    _Ret __stateful_in_order_recu(_Wrapper &&wrapper, _Ctx &ctx, _Tp &tp,
                                  _Ret ret) const {
        if constexpr (_Idx < std::tuple_size_v<std::decay_t<_Ctx>> - 1) {
            return __stateful_in_order_recu<_Ret, _Ctx, _Tp, _Idx + 1>(
                std::forward<_Wrapper>(wrapper), ctx, tp,
                wrapper(std::get<_Idx>(ctx), std::get<_Idx>(tp), ret));
        } else {
            return wrapper(std::get<_Idx>(ctx), std::get<_Idx>(tp), ret);
        }
    }
    static constexpr const auto __stateful_generate_cal_wrapper =
        [](const auto &ctx, auto &obj, std::size_t ret) {
            return obj.generate_calculate(ctx) + ret;
        };
    // end stateful generate calculate

    // begin generate
    template <typename... _Ts> struct __generate;

    template <std::size_t _Idx, typename _C, typename _T, typename _R>
    struct __generate_merge;
    template <std::size_t _Idx, typename _C, typename... _Ts, typename... _Rs>
    struct __generate_merge<_Idx, _C, std::tuple<_Ts...>, std::tuple<_Rs...>> {
        template <typename _Tp, typename Iter>
        Iter operator()(_Tp &&tp, Iter iter) const {
            if constexpr (sizeof...(_Rs) != 0) {
                return __generate<std::tuple_element_t<_Idx, _C>, _Ts...>()
                    .template f<
                        std::tuple_size_v<std::tuple_element_t<_Idx, _C>> - 1>(
                        std::get<_Idx>(tp),
                        __generate<_C, _Rs...>().template f<_Idx - 1>(tp,
                                                                      iter));
            } else {
                return __generate<std::tuple_element_t<_Idx, _C>, _Ts...>()
                    .template f<
                        std::tuple_size_v<std::tuple_element_t<_Idx, _C>> - 1>(
                        std::get<_Idx>(tp), iter);
            }
        }
    };

    template <typename _Ctx, typename _T, typename... _Ts>
    struct __generate<_Ctx, _T, _Ts...> {
        template <std::size_t _Idx, typename Iter>
        Iter f(_Ctx &ctx, Iter ite) const {
            if constexpr (!details::is_tuple<
                              std::tuple_element_t<_Idx, _Ctx>>::value) {
                if constexpr (sizeof...(_Ts) != 0) {
                    return _T().generate(
                        __generate<_Ctx, _Ts...>().template f<_Idx - 1>(ctx,
                                                                        ite),
                        __alter_get(std::get<_Idx>(ctx)));
                } else {
                    return _T().generate(ite, __alter_get(std::get<_Idx>(ctx)));
                }
            } else {
                if constexpr (sizeof...(_Ts) == 0) {
                    return __generate<std::tuple_element_t<_Idx, _Ctx>, _T>()
                        .template f<0>(std::get<_Idx>(ctx), ite);
                } else {
                    using __split = details::__split_tuple<
                        std::tuple_size_v<typename details::__tuple_expand<
                            std::tuple_element_t<_Idx, _Ctx>>::type>,
                        _T, _Ts...>;
                    return __generate_merge<_Idx, _Ctx,
                                            typename __split::prefix_type,
                                            typename __split::suffix_type>()(
                        ctx, ite);
                }
            }
        }
    };

    template <typename _Ctx, typename _Tp> struct __generate_impl;
    template <typename _Ctx, typename... _Ts>
    struct __generate_impl<_Ctx, std::tuple<_Ts...>>
        : __generate<_Ctx, _Ts...> {};
    // end generate

    // start stateful generate
    static constexpr const auto __stateful_generate_wrapper =
        [](const auto &ctx, auto &obj, auto iter) {
            return obj.generate(iter, ctx);
        };
    // end stateful generate

    // start stateless parse
    template <typename... _Ts> struct __parse;
    template <typename _Tp> struct __parse_merge;
    template <typename... _Ts> struct __parse_merge<std::tuple<_Ts...>> {
        template <std::size_t _Idx, typename Ite, typename _C>
        void f(_C &c, Ite &ite, bool &r, std::size_t &s) const {
            return __parse<_C, _Ts...>().template f<_Idx>(c, ite, r, s);
        }
    };

    template <typename _C, typename _T, typename... _Ts>
    struct __parse<_C, _T, _Ts...> {
        template <std::size_t _Idx, typename _Ite>
        void f(_C &ctx, _Ite &_ite, bool &r, std::size_t &s) {
            if (!r) {
                return;
            }

            if constexpr (!details::is_tuple<
                              std::tuple_element_t<_Idx, _C>>::value) {
                if constexpr (sizeof...(_Ts) != 0) {
                    _T().parse(_ite, __alter_get(std::get<_Idx>(ctx)), r, s);
                    return __parse<_C, _Ts...>().template f<_Idx + 1>(ctx, _ite,
                                                                      r, s);
                } else {
                    return _T().parse(_ite, __alter_get(std::get<_Idx>(ctx)), r,
                                      s);
                }
            } else {
                if constexpr (sizeof...(_Ts) != 0) {
                    using __split = details::__split_tuple<
                        std::tuple_size_v<typename details::__tuple_expand<
                            std::tuple_element_t<_Idx, _C>>::type>,
                        _T, _Ts...>;
                    __parse_merge<typename __split::prefix_type>()
                        .template f<0>(std::get<_Idx>(ctx), _ite, r, s);
                    if constexpr (std::tuple_size_v<
                                      typename __split::suffix_type> != 0) {
                        __parse_merge<typename __split::suffix_type>()
                            .template f<_Idx + 1>(ctx, _ite, r, s);
                    }
                    return;
                } else {
                    return __parse<std::tuple_element_t<_Idx, _C>, _T>()
                        .template f<0>(std::get<_Idx>(ctx), _ite, r, s);
                }
            }
        }
    };

    template <typename _C, typename _Tp> struct __parse_impl;
    template <typename _C, typename... _Ts>
    struct __parse_impl<_C, std::tuple<_Ts...>> : __parse<_C, _Ts...> {};
    // end stateless parse

    // start stateful parse
    template <std::size_t _Idx, typename _Ctx, typename _Tp, typename _Iter>
    void __stateful_parse_recu(_Iter &ite, _Ctx &_ctx, _Tp &_tp, bool &r,
                               std::size_t &s) const {
        if (!r) {
            return;
        }
        std::get<_Idx>(_tp).parse(ite, std::get<_Idx>(_ctx), r, s);
        if constexpr (_Idx < std::tuple_size_v<std::decay_t<_Ctx>> - 1) {
            return __stateful_parse_recu<_Idx + 1>(ite, _ctx, _tp, r, s);
        } else {
            return;
        }
    }
    // end stateful parse

  public:
    template <typename Ctx>
    std::size_t generate_calculate(const Ctx &ctx) const {
        return __generate_cal_impl<
                   const Ctx, typename details::__args_expand<Rules...>::type>()
            .template f<0>(ctx);
    }
    template <typename Ctx, typename Tp,
              typename = std::enable_if_t<details::is_tuple<Tp>::value>>
    std::size_t generate_calculate(const Ctx &ctx, Tp &tp) const {
        const auto flat_ctx = details::__flat_tp_const_ref(ctx);
        auto flat_tp = details::__flat_tp_ref(tp);
        return __stateful_in_order_recu<std::size_t, decltype(flat_ctx),
                                        decltype(flat_tp), 0>(
            __stateful_generate_cal_wrapper, flat_ctx, flat_tp, 0);
    }
    template <typename Ctx>
    std::size_t generate_calculate(const Ctx &ctx, unused_type) const {
        const auto flat_ctx = details::__flat_tp_const_ref(ctx);
        std::tuple<Rules...> __rs;
        auto flat_tp = details::__flat_tp_ref(__rs);
        return __stateful_in_order_recu<std::size_t, decltype(flat_ctx),
                                        decltype(flat_tp), 0>(
            __stateful_generate_cal_wrapper, flat_ctx, flat_tp, 0);
    }

    template <typename Iter, typename Ctx>
    Iter generate(Iter iter, const Ctx &ctx) const {
        using _Tp = typename details::__args_expand<Rules...>::type;
        using _Rp = details::__reverse_tuple<std::tuple_size_v<_Tp> - 1, _Tp>;
        return __generate_impl<const Ctx, typename _Rp::type>()
            .template f<std::tuple_size_v<Ctx> - 1>(ctx, iter);
    }
    template <typename Iter, typename Ctx>
    Iter generate(Iter iter, const Ctx &ctx, unused_type) const {
        std::tuple<Rules...> tp;
        const auto flat_ctx = details::__flat_tp_const_ref(ctx);
        auto flat_tp = details::__flat_tp_ref(tp);
        return __stateful_in_order_recu<Iter, decltype(flat_ctx),
                                        decltype(flat_tp), 0>(
            __stateful_generate_wrapper, flat_ctx, flat_tp, iter);
    }
    template <typename Iter, typename Ctx, typename Tp,
              typename = std::enable_if_t<details::is_tuple<Tp>::value>>
    Iter generate(Iter iter, const Ctx &ctx, Tp &tp) const {
        const auto flat_ctx = details::__flat_tp_const_ref(ctx);
        auto flat_tp = details::__flat_tp_ref(tp);
        return __stateful_in_order_recu<Iter, decltype(flat_ctx),
                                        decltype(flat_tp), 0>(
            __stateful_generate_wrapper, flat_ctx, flat_tp, iter);
    }

    template <typename Iter, typename Ctx>
    void parse(Iter &&ite, Ctx &ctx, bool &r, std::size_t &s) const {
        return __parse<Ctx, Rules...>().template f<0>(ctx, ite, r, s);
    }
    template <typename Iter, typename Ctx>
    bool parse(Iter &&ite, Ctx &ctx) const {
        bool r = true;
        std::size_t s = 0;
        __parse<Ctx, Rules...>().template f<0>(ctx, ite, r, s);
        return r;
    }

    template <
        typename Iter, typename Ctx, typename Tp,
        typename = std::enable_if_t<details::is_tuple<std::decay_t<Tp>>::value>>
    void parse(Iter &&ite, Ctx &ctx, bool &r, std::size_t &s, Tp &&tp) const {
        auto flat_ctx = details::__flat_tp_ref(ctx);
        auto flat_tp = details::__flat_tp_ref(tp);
        return __stateful_parse_recu<0>(ite, flat_ctx, flat_tp, r, s);
    }
    template <typename Iter, typename Ctx>
    void parse(Iter &&ite, Ctx &ctx, bool &r, std::size_t &s,
               unused_type) const {
        return parse(std::forward<Iter>(ite), ctx, r, s,
                     std::tuple<Rules...>{});
    }
    template <typename IterBegin, typename IterEnd, typename Ctx>
    bool parse(IterBegin &&begin, IterEnd &&end, Ctx &ctx) const {
        std::size_t size = std::distance(begin, end);
        bool r = true;
        parse(begin, ctx, r, size);
        return r;
    }
};

template <typename Origin, typename AlterRule, typename Extracter,
          typename StructTuple, typename Attribute>
class rule_t<alternative<Origin, AlterRule, Extracter, StructTuple, Attribute>>
    : public AlterRule {};

template <typename... L, typename... R>
constexpr rule_t<L..., R...> operator<<(rule_t<L...>,
                                        rule_t<R...>) noexcept(true) {
    return {};
}

namespace details {
template <typename _Tp> struct __deduce_rules;
template <typename... _Ts> struct __deduce_rules<std::tuple<_Ts...>> {
    using type = rule_t<rule_t<std::decay_t<_Ts>>...>;
};
}  // namespace details

template <typename Context>
using auto_rule_t = typename details::__deduce_rules<
    typename details::__tp_expand<Context>::type>::type;
}  // namespace chx::serialization