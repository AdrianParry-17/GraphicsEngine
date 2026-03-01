#pragma once

#include <cmath>

namespace Engine {
    /// @brief Numeric tolerance constants and helpers used across the engine.
    class NumericConstants final {
    public:
        /// @brief Threshold for near-zero floating-point comparisons.
        static constexpr double NearZero = 1E-8;

        /// @brief Checks whether a value is near zero.
        /// @param x Value to test.
        /// @return True if |x| < NearZero.
        static bool IsNearZero(double x) { return std::abs(x) < NumericConstants::NearZero; }

        /// @brief Checks whether a value is negative or near zero.
        /// @param x Value to test.
        /// @return True if x < NearZero.
        static bool IsNearZeroOrNegative(double x) { return x < NumericConstants::NearZero; }

        /// @brief Checks whether a value is positive or near zero.
        /// @param x Value to test.
        /// @return True if x > -NearZero.
        static bool IsNearZeroOrPositive(double x) { return x > -NumericConstants::NearZero; }
    };
}