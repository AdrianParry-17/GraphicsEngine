#pragma once

#include <cmath>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace Tests {
    struct TestCase {
        std::string name;
        std::function<void()> func;
    };

    inline std::vector<TestCase>& Registry() {
        static std::vector<TestCase> tests;
        return tests;
    }

    inline void Register(const std::string& name, const std::function<void()>& func) {
        Registry().push_back(TestCase{name, func});
    }

    inline void Fail(const std::string& message) {
        throw std::runtime_error(message);
    }
}

#define TEST_CASE(Name) \
    static void Name(); \
    namespace { \
        struct Name##_Registrar { \
            Name##_Registrar() { Tests::Register(#Name, Name); } \
        } Name##_registrar_instance; \
    } \
    static void Name()

#define EXPECT_TRUE(Expr) \
    do { \
        if (!(Expr)) { \
            std::ostringstream _oss; \
            _oss << "EXPECT_TRUE failed: " #Expr; \
            Tests::Fail(_oss.str()); \
        } \
    } while (0)

#define EXPECT_EQ(Expected, Actual) \
    do { \
        auto _expected = (Expected); \
        auto _actual = (Actual); \
        if (!(_expected == _actual)) { \
            std::ostringstream _oss; \
            _oss << "EXPECT_EQ failed: expected=" << _expected << " actual=" << _actual; \
            Tests::Fail(_oss.str()); \
        } \
    } while (0)

#define EXPECT_NEAR(Expected, Actual, Eps) \
    do { \
        auto _expected = (Expected); \
        auto _actual = (Actual); \
        auto _eps = (Eps); \
        if (std::fabs(_expected - _actual) > _eps) { \
            std::ostringstream _oss; \
            _oss << "EXPECT_NEAR failed: expected=" << _expected << " actual=" << _actual << " eps=" << _eps; \
            Tests::Fail(_oss.str()); \
        } \
    } while (0)
