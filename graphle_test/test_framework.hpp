#pragma once

#include <vector>
#include <string_view>
#include <iostream>
#include <source_location>
#include <format>
#include <functional>


#define GRAPHLE_CAT_1(A, B) A##B
#define GRAPHLE_CAT(A, B) GRAPHLE_CAT_1(A, B)


/**
 * @ingroup Config
 * @def GRAPHLE_TEST_DONT_CATCH_ERRORS
 * Don't catch errors during testing so they can trigger an IDE breakpoint instead.
 */
#ifdef GRAPHLE_DOXYGEN
    #define GRAPHLE_TEST_DONT_CATCH_ERRORS "Not Defined"
#endif


/**
 * @def TEST(Suite, Name)
 * Defines a new test with the given suite and name which will be invoked when tests are run.
 */
#define TEST(Suite, Name)                                       \
struct test_##Suite_##Name {                                    \
    static void main(void);                                     \
                                                                \
    static inline auto autoregister = [] {                      \
        graphle::test::test_registry::instance()                \
            .register_test(                                     \
                #Suite,                                         \
                #Name,                                          \
                &test_##Suite_##Name::main);                    \
                                                                \
        return 0;                                               \
    } ();                                                       \
};                                                              \
                                                                \
void test_##Suite_##Name::main(void)


/**
 * @def SUBTEST_SCOPE(Name)
 * Defines a subscope of the current test which will have a separate success or error message logged.
 */
#define SUBTEST_SCOPE(Name) \
if (auto impl_scope_object = graphle::test::subtest_scope_handler { Name }; true)



/**
 * @def FAIL(...)
 * Fails the current test with the given message.
 */
#define FAIL(...) \
throw graphle::test::assertion_failure { __VA_ARGS__ }

/**
 * @def ASSERT_TRUE(...)
 * Fails the current test if the given condition is not true.
 */
#define ASSERT_TRUE(...) \
if (!bool(__VA_ARGS__)) FAIL("Condition was not true: " #__VA_ARGS__)

/**
 * @def ASSERT_FALSE(...)
 * Fails the current test if the given condition is true.
 */
#define ASSERT_FALSE(...) \
if (bool(__VA_ARGS__)) FAIL("Condition was not false: " #__VA_ARGS__)


namespace graphle::test {
    template <typename R, typename... A> using fn = R(*)(A...);

    constexpr static inline const char* color_red     = "\x1B[31m";
    constexpr static inline const char* color_green   = "\x1B[32m";
    constexpr static inline const char* color_restore = "\033[0m";


    /** Exception thrown when a test fails. */
    struct assertion_failure : std::exception {
        explicit assertion_failure(std::string_view condition, std::source_location where = std::source_location::current()) :
            condition(condition),
            where(where),
            message(std::format("Assertion failure at line {} in {} ({}): {}", where.line(), where.file_name(), where.function_name(), condition))
        {}

        const char* what(void) const noexcept override {
            return message.c_str();
        }

        std::string_view condition;
        std::source_location where;
        std::string message;
    };


    /** Print success message for the test with the given name. */
    inline void emit_test_success(std::string_view name) {
        printf(color_green);
        std::cout << "[Test " << name << "]: SUCCESS\n";
        printf(color_restore);
    }


    /** Print error message for the test with the given name using the given exception. */
    inline void emit_test_failure(std::string_view name, const auto& failure) {
        printf(color_red);
        std::cout << "[Test " << name << "]: FAILED\n\t" << failure.what() << "\n";
        printf(color_restore);
    }


    /** Registry to keep track of tests. */
    class test_registry {
    public:
        static test_registry& instance(void) {
            static test_registry i { };
            return i;
        }


        void register_test(std::string_view suite, std::string_view name, fn<void> test) {
            tests.emplace_back(test, std::format("{}::{}", suite, name));
        }


        int run_tests(void) {
            bool has_failures = false;


            for (auto& test_data : tests) {
                auto& [test, name, scope] = test_data;
                current = std::addressof(test_data);


                #ifdef GRAPHLE_TEST_DONT_CATCH_ERRORS
                    struct never_thrown_t { const char* what(void) const { return nullptr; } };
                    using exception_t = never_thrown_t;
                #else
                    using exception_t = std::exception;
                #endif


                try {
                    std::invoke(test);
                    emit_test_success(name);
                    scope.clear();
                } catch (const exception_t& e) {
                    has_failures = true;

                    while (!scope.empty()) {
                        emit_test_failure(test_data.scoped_name(), e);
                        scope.pop_back();
                    }

                    emit_test_failure(name, e);
                }
            }

            return has_failures ? EXIT_FAILURE : EXIT_SUCCESS;
        }


        void begin_subtest(std::string name) {
            current->subtest_scope.push_back(std::move(name));
        }


        void end_subtest(void) {
            emit_test_success(current->scoped_name());
            current->subtest_scope.pop_back();
        }


        const auto& current_test(void) const {
            return *current;
        }
    private:
        struct test_data {
            fn<void> test;
            std::string name;
            std::vector<std::string> subtest_scope;


            [[nodiscard]] std::string scoped_name(void) const {
                std::string result = name;

                for (const auto& scope : subtest_scope) {
                    result += "::";
                    result += scope;
                }

                return result;
            }
        };


        std::vector<test_data> tests;
        test_data* current = nullptr;
    };


    /** RAII object to start and end a subtest scope. */
    struct subtest_scope_handler {
        explicit subtest_scope_handler(std::string name) {
            test_registry::instance().begin_subtest(std::move(name));
        }

        ~subtest_scope_handler(void) {
            if (!std::uncaught_exceptions()) test_registry::instance().end_subtest();
        }
    };
}


/** Test framework entry point. Runs all registered tests. */
int main(void) {
    return graphle::test::test_registry::instance().run_tests();
}