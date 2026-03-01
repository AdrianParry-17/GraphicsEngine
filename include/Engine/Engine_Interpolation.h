#pragma once

#include <functional>

namespace Engine {
    template <typename T>
    /// @brief Generic interpolation interface.
    /// @tparam T Interpolated value type.
    struct IInterpolator {
        /// @brief Virtual destructor.
        virtual ~IInterpolator() = default;

        /// @brief Performs linear interpolation between two values.
        /// @param a Start value.
        /// @param b End value.
        /// @param t Interpolation factor in [0, 1] (caller-defined contract).
        /// @param result Output interpolated value.
        virtual void Linear(const T& a, const T& b, double t, T& result) = 0;

        /// @brief Performs barycentric interpolation across a triangle.
        /// @param a First corner value.
        /// @param b Second corner value.
        /// @param c Third corner value.
        /// @param wa Weight for a.
        /// @param wb Weight for b.
        /// @param wc Weight for c.
        /// @param result Output interpolated value.
        virtual void Triangle(const T& a, const T& b, const T& c, double wa, double wb, double wc, T& result) = 0;
    };

    /// @brief Default arithmetic interpolator implementation.
    /// @tparam T Interpolated value type.
    template <typename T>
    class Interpolator : public IInterpolator<T>{
    public:
        /// @brief Linearly interpolates using weighted sum.
        void Linear(const T& a, const T& b, double t, T& result) override {
            result = ((a * (1 - t)) + (b * t));
        }

        /// @brief Interpolates triangle value using barycentric weights.
        void Triangle(const T& a, const T& b, const T& c, double wa, double wb, double wc, T& result) override {
            result = ((a * wa) + (b * wb) + (c * wc));
        }
    };

    /// @brief Function-backed interpolator for customizable interpolation logic.
    /// @tparam T Interpolated value type.
    template <typename T>
    class FunctionInterpolator : public IInterpolator<T> {
    private:
        std::function<void(const T&, const T&, double, T&)> linear_func;
        std::function<void(const T&, const T&, const T&, double, double, double, T&)> triangle_func;
    public:
        /// @brief Constructs a function-backed interpolator.
        /// @param _linear_func Callback for linear interpolation.
        /// @param _triangle_func Callback for triangle interpolation.
        FunctionInterpolator(
            const std::function<void(const T&, const T&, double, T&)>& _linear_func,
            const std::function<void(const T&, const T&, const T&, double, double, double, T&)>& _triangle_func)
            : linear_func(_linear_func), triangle_func(_triangle_func) {}
        
        /// @brief Replaces the linear interpolation callback.
        /// @param func New linear callback.
        void SetLinear(const std::function<void(const T&, const T&, double, T&)>& func) { linear_func = func; }

        /// @brief Replaces the triangle interpolation callback.
        /// @param func New triangle callback.
        void SetTriangle(const std::function<void(const T&, const T&, const T&, double, double, double, T&)>& func) { triangle_func = func; }

        /// @brief Executes linear interpolation callback.
        void Linear(const T& a, const T& b, double t, T& result) override { linear_func(a, b, t, result); }

        /// @brief Executes triangle interpolation callback.
        void Triangle(const T& a, const T& b, const T& c, double wa, double wb, double wc, T& result) override { triangle_func(a, b, c, wa, wb, wc, result); }
    };
}
