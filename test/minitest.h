// Minimal, dependency-free unit test framework.
//
// Used instead of pulling in a third-party framework so the whole test
// suite builds with nothing but a C++17 compiler - no package manager,
// no network access, no PlatformIO/Arduino toolchain required.
#pragma once

#include <cmath>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace minitest {

struct TestCase {
    std::string name;
    std::function<void()> fn;
};

inline std::vector<TestCase>& registry() {
    static std::vector<TestCase> tests;
    return tests;
}

struct Registrar {
    Registrar(const std::string& name, const std::function<void()>& fn) {
        registry().push_back({name, fn});
    }
};

inline int failureCount = 0;
inline int assertionCount = 0;
inline std::string currentTestName;
inline bool currentTestFailed = false;

inline void reportFailure(const std::string& expr, const char* file, int line) {
    failureCount++;
    currentTestFailed = true;
    std::cerr << "  [FAIL] " << currentTestName << " (" << file << ":" << line
               << "): " << expr << "\n";
}

}  // namespace minitest

#define TEST_CASE(name)                                                   \
    static void name();                                                  \
    static minitest::Registrar registrar_##name(#name, name);            \
    static void name()

#define ASSERT_TRUE(cond)                                                 \
    do {                                                                  \
        minitest::assertionCount++;                                      \
        if (!(cond)) minitest::reportFailure(#cond, __FILE__, __LINE__);  \
    } while (0)

#define ASSERT_EQ(a, b)                                                   \
    do {                                                                  \
        minitest::assertionCount++;                                      \
        if (!((a) == (b)))                                                \
            minitest::reportFailure(std::string(#a " == " #b), __FILE__,  \
                                     __LINE__);                           \
    } while (0)

#define ASSERT_NEAR(a, b, tol)                                            \
    do {                                                                  \
        minitest::assertionCount++;                                      \
        if (std::fabs(static_cast<double>(a) - static_cast<double>(b)) >  \
            (tol))                                                        \
            minitest::reportFailure(std::string(#a " ~= " #b), __FILE__,  \
                                     __LINE__);                           \
    } while (0)
