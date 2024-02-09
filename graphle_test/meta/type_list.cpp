#include <graphle.hpp>
#include <test_framework.hpp>

#include <type_traits>
#include <tuple>
#include <typeinfo>
#include <typeindex>
#include <algorithm>


#define ASSERT_TYPE_EQ(...) \
ASSERT_TRUE(std::is_same_v<__VA_ARGS__>)


using graphle::meta::type_list;
using list_empty = type_list<>;
using list_i     = type_list<int>;
using list_ic    = type_list<int, char>;
using list_icb   = type_list<int, char, bool>;


TEST(type_list, head) {
    ASSERT_TYPE_EQ(list_empty::head, graphle::meta::none);
    ASSERT_TYPE_EQ(list_i::head,     int);
    ASSERT_TYPE_EQ(list_ic::head,    int);
    ASSERT_TYPE_EQ(list_icb::head,   int);
}


TEST(type_list, tail) {
    ASSERT_TYPE_EQ(list_empty::tail, type_list<>);
    ASSERT_TYPE_EQ(list_i::tail,     type_list<>);
    ASSERT_TYPE_EQ(list_ic::tail,    type_list<char>);
    ASSERT_TYPE_EQ(list_icb::tail,   type_list<char, bool>);
}


TEST(type_list, map) {
    ASSERT_TYPE_EQ(list_empty::template map<std::add_const_t>, type_list<>);
    ASSERT_TYPE_EQ(list_i::template map<std::add_const_t>,     type_list<const int>);
    ASSERT_TYPE_EQ(list_ic::template map<std::add_const_t>,    type_list<const int, const char>);
    ASSERT_TYPE_EQ(list_icb::template map<std::add_const_t>,   type_list<const int, const char, const bool>);
}


TEST(type_list, to) {
    ASSERT_TYPE_EQ(list_empty::template to<std::tuple>, std::tuple<>);
    ASSERT_TYPE_EQ(list_i::template to<std::tuple>,     std::tuple<int>);
    ASSERT_TYPE_EQ(list_ic::template to<std::tuple>,    std::tuple<int, char>);
    ASSERT_TYPE_EQ(list_icb::template to<std::tuple>,   std::tuple<int, char, bool>);
}


TEST(type_list, prepend) {
    ASSERT_TYPE_EQ(list_empty::template prepend<void>, type_list<void>);
    ASSERT_TYPE_EQ(list_i::template prepend<void>,     type_list<void, int>);
    ASSERT_TYPE_EQ(list_ic::template prepend<void>,    type_list<void, int, char>);
    ASSERT_TYPE_EQ(list_icb::template prepend<void>,   type_list<void, int, char, bool>);
}


TEST(type_list, append) {
    ASSERT_TYPE_EQ(list_empty::template append<void>, type_list<void>);
    ASSERT_TYPE_EQ(list_i::template append<void>,     type_list<int, void>);
    ASSERT_TYPE_EQ(list_ic::template append<void>,    type_list<int, char, void>);
    ASSERT_TYPE_EQ(list_icb::template append<void>,   type_list<int, char, bool, void>);
}


TEST(type_list, prepend_list) {
    SUBTEST_SCOPE("empty") {
        using L = type_list<>;
        
        ASSERT_TYPE_EQ(list_empty::template prepend_list<L>, type_list<>);
        ASSERT_TYPE_EQ(list_i::template prepend_list<L>,     type_list<int>);
        ASSERT_TYPE_EQ(list_ic::template prepend_list<L>,    type_list<int, char>);
        ASSERT_TYPE_EQ(list_icb::template prepend_list<L>,   type_list<int, char, bool>);
    }


    SUBTEST_SCOPE("size 1") {
        using L = type_list<void>;

        ASSERT_TYPE_EQ(list_empty::template prepend_list<L>, type_list<void>);
        ASSERT_TYPE_EQ(list_i::template prepend_list<L>,     type_list<void, int>);
        ASSERT_TYPE_EQ(list_ic::template prepend_list<L>,    type_list<void, int, char>);
        ASSERT_TYPE_EQ(list_icb::template prepend_list<L>,   type_list<void, int, char, bool>);
    }


    SUBTEST_SCOPE("size 2") {
        using L = type_list<void, void>;

        ASSERT_TYPE_EQ(list_empty::template prepend_list<L>, type_list<void, void>);
        ASSERT_TYPE_EQ(list_i::template prepend_list<L>,     type_list<void, void, int>);
        ASSERT_TYPE_EQ(list_ic::template prepend_list<L>,    type_list<void, void, int, char>);
        ASSERT_TYPE_EQ(list_icb::template prepend_list<L>,   type_list<void, void, int, char, bool>);
    }
}


TEST(type_list, append_list) {
    SUBTEST_SCOPE("empty") {
        using L = type_list<>;

        ASSERT_TYPE_EQ(list_empty::template append_list<L>, type_list<>);
        ASSERT_TYPE_EQ(list_i::template append_list<L>,     type_list<int>);
        ASSERT_TYPE_EQ(list_ic::template append_list<L>,    type_list<int, char>);
        ASSERT_TYPE_EQ(list_icb::template append_list<L>,   type_list<int, char, bool>);
    }


    SUBTEST_SCOPE("size 1") {
        using L = type_list<void>;

        ASSERT_TYPE_EQ(list_empty::template append_list<L>, type_list<void>);
        ASSERT_TYPE_EQ(list_i::template append_list<L>,     type_list<int, void>);
        ASSERT_TYPE_EQ(list_ic::template append_list<L>,    type_list<int, char, void>);
        ASSERT_TYPE_EQ(list_icb::template append_list<L>,   type_list<int, char, bool, void>);
    }


    SUBTEST_SCOPE("size 2") {
        using L = type_list<void, void>;

        ASSERT_TYPE_EQ(list_empty::template append_list<L>, type_list<void, void>);
        ASSERT_TYPE_EQ(list_i::template append_list<L>,     type_list<int, void, void>);
        ASSERT_TYPE_EQ(list_ic::template append_list<L>,    type_list<int, char, void, void>);
        ASSERT_TYPE_EQ(list_icb::template append_list<L>,   type_list<int, char, bool, void, void>);
    }
}


TEST(type_list, apply) {
    list_empty::apply([] <typename... Ts> () {
        ASSERT_TRUE(sizeof...(Ts) == 0);
    });

    list_i::apply([] <typename... Ts> () {
        ASSERT_TYPE_EQ(type_list<Ts...>, list_i);
    });

    list_ic::apply([] <typename... Ts> () {
        ASSERT_TYPE_EQ(type_list<Ts...>, list_ic);
    });

    list_icb::apply([] <typename... Ts> () {
        ASSERT_TYPE_EQ(type_list<Ts...>, list_icb);
    });
}


TEST(type_list, foreach) {
    std::vector<std::type_index> seen;

    auto check_seen = [&] (std::initializer_list<std::type_index> ts) {
        for (const auto& t : ts) {
            ASSERT_FALSE(std::ranges::find(seen, t) == seen.end());
        }

        seen.clear();
    };


    list_empty::foreach([] <typename T> () {
        FAIL("Invoked apply on empty list.");
    });


    list_i::foreach([&] <typename T> () {
        seen.emplace_back(typeid(T));
    });

    check_seen({ typeid(int) });


    list_ic::foreach([&] <typename T> () {
        seen.emplace_back(typeid(T));
    });

    check_seen({ typeid(int), typeid(char) });


    list_icb::foreach([&] <typename T> () {
        seen.emplace_back(typeid(T));
    });

    check_seen({ typeid(int), typeid(char), typeid(bool) });
}


TEST(type_list, pop_front) {
    SUBTEST_SCOPE("none") {
        ASSERT_TYPE_EQ(list_empty::template pop_front<0>, type_list<>);
        ASSERT_TYPE_EQ(list_i::template pop_front<0>,     type_list<int>);
        ASSERT_TYPE_EQ(list_ic::template pop_front<0>,    type_list<int, char>);
        ASSERT_TYPE_EQ(list_icb::template pop_front<0>,   type_list<int, char, bool>);
    }


    SUBTEST_SCOPE("one") {
        ASSERT_TYPE_EQ(list_empty::template pop_front<1>, type_list<>);
        ASSERT_TYPE_EQ(list_i::template pop_front<1>,     type_list<>);
        ASSERT_TYPE_EQ(list_ic::template pop_front<1>,    type_list<char>);
        ASSERT_TYPE_EQ(list_icb::template pop_front<1>,   type_list<char, bool>);
    }


    SUBTEST_SCOPE("multiple") {
        ASSERT_TYPE_EQ(list_empty::template pop_front<2>, type_list<>);
        ASSERT_TYPE_EQ(list_i::template pop_front<2>,     type_list<>);
        ASSERT_TYPE_EQ(list_ic::template pop_front<2>,    type_list<>);
        ASSERT_TYPE_EQ(list_icb::template pop_front<2>,   type_list<bool>);
    }
}


TEST(type_list, pop_back) {
    SUBTEST_SCOPE("none") {
        ASSERT_TYPE_EQ(list_empty::template pop_back<0>, type_list<>);
        ASSERT_TYPE_EQ(list_i::template pop_back<0>,     type_list<int>);
        ASSERT_TYPE_EQ(list_ic::template pop_back<0>,    type_list<int, char>);
        ASSERT_TYPE_EQ(list_icb::template pop_back<0>,   type_list<int, char, bool>);
    }


    SUBTEST_SCOPE("one") {
        ASSERT_TYPE_EQ(list_empty::template pop_back<1>, type_list<>);
        ASSERT_TYPE_EQ(list_i::template pop_back<1>,     type_list<>);
        ASSERT_TYPE_EQ(list_ic::template pop_back<1>,    type_list<int>);
        ASSERT_TYPE_EQ(list_icb::template pop_back<1>,   type_list<int, char>);
    }


    SUBTEST_SCOPE("multiple") {
        ASSERT_TYPE_EQ(list_empty::template pop_back<2>, type_list<>);
        ASSERT_TYPE_EQ(list_i::template pop_back<2>,     type_list<>);
        ASSERT_TYPE_EQ(list_ic::template pop_back<2>,    type_list<>);
        ASSERT_TYPE_EQ(list_icb::template pop_back<2>,   type_list<int>);
    }
}


TEST(type_list, erase_at) {
    SUBTEST_SCOPE("first") {
        ASSERT_TYPE_EQ(list_i::template erase_at<0>,     type_list<>);
        ASSERT_TYPE_EQ(list_ic::template erase_at<0>,    type_list<char>);
        ASSERT_TYPE_EQ(list_icb::template erase_at<0>,   type_list<char, bool>);
    }


    SUBTEST_SCOPE("last") {
        ASSERT_TYPE_EQ(list_i::template erase_at<0>,     type_list<>);
        ASSERT_TYPE_EQ(list_ic::template erase_at<1>,    type_list<int>);
        ASSERT_TYPE_EQ(list_icb::template erase_at<2>,   type_list<int, char>);
    }


    SUBTEST_SCOPE("middle") {
        ASSERT_TYPE_EQ(list_icb::template erase_at<1>,   type_list<int, bool>);
    }
}


TEST(type_list, insert_at) {
    SUBTEST_SCOPE("first") {
        ASSERT_TYPE_EQ(list_empty::template insert_at<void, 0>, type_list<void>);
        ASSERT_TYPE_EQ(list_i::template insert_at<void, 0>,     type_list<void, int>);
        ASSERT_TYPE_EQ(list_ic::template insert_at<void, 0>,    type_list<void, int, char>);
        ASSERT_TYPE_EQ(list_icb::template insert_at<void, 0>,   type_list<void, int, char, bool>);
    }


    SUBTEST_SCOPE("last") {
        ASSERT_TYPE_EQ(list_empty::template insert_at<void, 0>, type_list<void>);
        ASSERT_TYPE_EQ(list_i::template insert_at<void, 1>,     type_list<int, void>);
        ASSERT_TYPE_EQ(list_ic::template insert_at<void, 2>,    type_list<int, char, void>);
        ASSERT_TYPE_EQ(list_icb::template insert_at<void, 3>,   type_list<int, char, bool, void>);
    }


    SUBTEST_SCOPE("middle") {
        ASSERT_TYPE_EQ(list_ic::template insert_at<void, 1>,    type_list<int, void, char>);
        ASSERT_TYPE_EQ(list_icb::template insert_at<void, 1>,   type_list<int, void, char, bool>);
    }
}


template <typename... Ts> struct is_one_of {
    template <typename X> struct type {
        constexpr static inline bool value = (std::is_same_v<X, Ts> || ...);
    };
};


TEST(type_list, filter) {
    SUBTEST_SCOPE("all_true_trait") {
        ASSERT_TYPE_EQ(list_empty::template filter_trait<std::is_integral>, type_list<>);
        ASSERT_TYPE_EQ(list_i::template filter_trait<std::is_integral>,     type_list<int>);
        ASSERT_TYPE_EQ(list_ic::template filter_trait<std::is_integral>,    type_list<int, char>);
        ASSERT_TYPE_EQ(list_icb::template filter_trait<std::is_integral>,   type_list<int, char, bool>);
    }


    SUBTEST_SCOPE("one_true_trait") {
        ASSERT_TYPE_EQ(list_empty::template filter_trait<is_one_of<int>::template type>, type_list<>);
        ASSERT_TYPE_EQ(list_i::template filter_trait<is_one_of<int>::template type>,     type_list<int>);
        ASSERT_TYPE_EQ(list_ic::template filter_trait<is_one_of<int>::template type>,    type_list<int>);
        ASSERT_TYPE_EQ(list_icb::template filter_trait<is_one_of<int>::template type>,   type_list<int>);
    }


    SUBTEST_SCOPE("some_true_trait") {
        ASSERT_TYPE_EQ(list_empty::template filter_trait<is_one_of<int, bool>::template type>, type_list<>);
        ASSERT_TYPE_EQ(list_i::template filter_trait<is_one_of<int, bool>::template type>,     type_list<int>);
        ASSERT_TYPE_EQ(list_ic::template filter_trait<is_one_of<int, bool>::template type>,    type_list<int>);
        ASSERT_TYPE_EQ(list_icb::template filter_trait<is_one_of<int, bool>::template type>,   type_list<int, bool>);
    }


    SUBTEST_SCOPE("all_false_trait") {
        ASSERT_TYPE_EQ(list_empty::template filter_trait<is_one_of<std::string>::template type>, type_list<>);
        ASSERT_TYPE_EQ(list_i::template filter_trait<is_one_of<std::string>::template type>,     type_list<>);
        ASSERT_TYPE_EQ(list_ic::template filter_trait<is_one_of<std::string>::template type>,    type_list<>);
        ASSERT_TYPE_EQ(list_icb::template filter_trait<is_one_of<std::string>::template type>,   type_list<>);
    }
}