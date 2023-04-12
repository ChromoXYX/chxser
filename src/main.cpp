#include <bitset>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <variant>

#include "../include/chx/serialization/rule.hpp"
#include "../include/chx/serialization/struct_tuple.hpp"
#include "../include/chx/serialization/types/fundamental.hpp"
#include "../include/chx/serialization/types/list.hpp"
#include "../include/chx/serialization/types/optional.hpp"
#include "../include/chx/serialization/types/variant.hpp"

namespace ser = chx::serialization;

struct B {
    int i = 42;
};

struct A {
    double d = 3.14;
    std::vector<B> v;
};

int main(void) {
    A a;
    a.v.resize(10);
    using STPB = ser::as<&B::i>;
    using RB = ser::auto_rule_t<ser::auto_tuple_t<B, &B::i>>;
    using STPA =
        ser::struct_tuple<ser::as<&A::d>,
                          ser::as_alter<A&, RB, ser::default_extractor<&A::v>,
                                        STPB, ser::attrs::ListReplace>>;
    auto refa = STPA()(a);
    ser::auto_rule_t<decltype(refa)> ra;
    std::string str;
    ra.generate(std::back_inserter(str), refa);
    std::get<1>(refa).get().clear();
    auto begin = std::chrono::high_resolution_clock::now();
    // for (int i = 0; i < 1000000; ++i) {
    ra.parse(str.begin(), str.end(), refa);
    a.v.clear();
    //}
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       begin)
                     .count()
              << "ms\n";
    std::cout << str.size() << "\n";
}