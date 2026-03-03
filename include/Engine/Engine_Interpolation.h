#pragma once

#include <functional>

namespace Engine {
    /// @brief Generic interpolation interface.
    /// @tparam T Interpolated value type.
    template <typename T>
    struct IInterpolator {
        /// @brief Virtual destructor.
        virtual ~IInterpolator() = default;

        /// @brief Performs scaling interpolation of the given value with a scalar.
        /// @param in Input value.
        /// @param scalar The scalar factor.
        /// @param result Output scaled value.
        virtual void Scale(const T& in, double scalar, T& result) = 0;

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
        void Scale(const T& in, double scalar, T& result) override {
            result = in * scalar;
        }

        /// @brief Linearly interpolates using weighted sum.
        void Linear(const T& a, const T& b, double t, T& result) override {
            result = ((a * (1 - t)) + (b * t));
        }

        /// @brief Interpolates triangle value using barycentric weights.
        void Triangle(const T& a, const T& b, const T& c, double wa, double wb, double wc, T& result) override {
            result = ((a * wa) + (b * wb) + (c * wc));
        }
    };

    template <typename T>
    class IVectorInterpolator : public IInterpolator<T> {
    public:
        /// @brief Perform a scalar multiplication
        /// @param in The input value.
        /// @param scalar The scalar.
        /// @param result The output result.
        virtual void ScalarMultiply(const T& in, double scalar, T& result) = 0;
        /// @brief Perform an addition
        /// @param a The left side value of the addition.
        /// @param b The right side value of the addition.
        /// @param result The output result.
        virtual void Add(const T& a, const T& b, T& result) = 0;

        void Scale(const T& in, double scalar, T& result) override { ScalarMultiply(in, scalar, result); }
        void Linear(const T& a, const T& b, double t, T& result) override {
            // Split to prevent (re-read input after assigning result) problem
            T left, right; // Required default constructor
            ScalarMultiply(a, 1 - t, left);
            ScalarMultiply(b, t, right);
            Add(left, right, result);
        }
        void Triangle(const T& a, const T& b, const T& c, double wa, double wb, double wc, T& result) override {
            // Split to prevent (re-read input after assigning result) problem
            T tmp1, tmp2; // Required default constructor
            ScalarMultiply(a, wa, tmp1); // tmp1 = a * wa
            ScalarMultiply(b, wb, result); // result = b * wb
            Add(tmp1, result, tmp2); // tmp2 = tmp1 + result = a * wa + b * wb

            ScalarMultiply(c, wc, tmp1); // tmp1 = c * wc
            Add(tmp2, tmp1, result); // result = tmp2 + tmp1 = a * wa + b * wb + c * wc
        }
    };

    template <typename T>
    class VectorInterpolator : public IVectorInterpolator<T> {
    public:
        void ScalarMultiply(const T& in, double scalar, T& result) override { result = in * scalar; }
        void Add(const T& a, const T& b, T& result) override { result = a + b; }
    };

    template <typename T>
    class FunctionVectorInterpolator : public IVectorInterpolator<T> {
    private:
        std::function<void(const T&, double, T&)> scalar_multiply_func;
        std::function<void(const T&, const T&, T&)> add_func;
    public:
        /// @brief Constructs a function-backed vector interpolator.
        /// @param _scalar_multiply_func Callback for scalar multiplication.
        /// @param _add_func Callback for addition.
        FunctionVectorInterpolator(
            const std::function<void(const T&, double, T&)>& _scalar_multiply_func,
            const std::function<void(const T&, const T&, T&)>& _add_func)
            : scalar_multiply_func(_scalar_multiply_func), add_func(_add_func) {}

        /// @brief Replaces the scalar multiplication callback.
        /// @param func New scalar multiplication callback.
        void SetScalarMultiply(const std::function<void(const T&, double, T&)>& func) { scalar_multiply_func = func; }

        /// @brief Replaces the addition callback.
        /// @param func New addition callback.
        void SetAddition(const std::function<void(const T&, const T&, T&)>& func) { add_func = func; }

        void ScalarMultiply(const T& in, double scalar, T& result) override { scalar_multiply_func(in, scalar, result); }
        void Add(const T& a, const T& b, T& result) override { add_func(a, b, result); }
    };

    /// @brief Function-backed interpolator for customizable interpolation logic.
    /// @tparam T Interpolated value type.
    template <typename T>
    class FunctionInterpolator : public IInterpolator<T> {
    private:
        std::function<void(const T&, double, T&)> scale_func;
        std::function<void(const T&, const T&, double, T&)> linear_func;
        std::function<void(const T&, const T&, const T&, double, double, double, T&)> triangle_func;
    public:
        /// @brief Constructs a function-backed interpolator.
        /// @param _scale_func Callback for scaling.
        /// @param _linear_func Callback for linear interpolation.
        /// @param _triangle_func Callback for triangle interpolation.
        FunctionInterpolator(
            const std::function<void(const T&, double, T&)>& _scale_func,
            const std::function<void(const T&, const T&, double, T&)>& _linear_func,
            const std::function<void(const T&, const T&, const T&, double, double, double, T&)>& _triangle_func)
            : scale_func(_scale_func), linear_func(_linear_func), triangle_func(_triangle_func) {}
        
        /// @brief Replaces the scale callback.
        /// @param func New scale callback.
        void SetScale(const std::function<void(const T&, double, T&)>& func) { scale_func = func; }

        /// @brief Replaces the linear interpolation callback.
        /// @param func New linear callback.
        void SetLinear(const std::function<void(const T&, const T&, double, T&)>& func) { linear_func = func; }

        /// @brief Replaces the triangle interpolation callback.
        /// @param func New triangle callback.
        void SetTriangle(const std::function<void(const T&, const T&, const T&, double, double, double, T&)>& func) { triangle_func = func; }

        void Scale(const T& in, double scalar, T& result) override { scale_func(in, scalar, result); }
        void Linear(const T& a, const T& b, double t, T& result) override { linear_func(a, b, t, result); }
        void Triangle(const T& a, const T& b, const T& c, double wa, double wb, double wc, T& result) override { triangle_func(a, b, c, wa, wb, wc, result); }
    };
}
