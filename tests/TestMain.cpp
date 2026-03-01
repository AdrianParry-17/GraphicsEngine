#include "TestHarness.h"

#include <exception>
#include <iostream>

int main() {
    const auto& tests = Tests::Registry();
    int passed = 0;
    int failed = 0;

    for (const auto& test : tests) {
        try {
            test.func();
            ++passed;
            std::cout << "[PASS] " << test.name << '\n';
        } catch (const std::exception& e) {
            ++failed;
            std::cout << "[FAIL] " << test.name << " - " << e.what() << '\n';
        } catch (...) {
            ++failed;
            std::cout << "[FAIL] " << test.name << " - unknown exception\n";
        }
    }

    std::cout << "\nTotal: " << tests.size() << ", Passed: " << passed << ", Failed: " << failed << '\n';
    return failed == 0 ? 0 : 1;
}
