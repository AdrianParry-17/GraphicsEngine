#pragma once

#include <functional>

namespace Engine {
    /**
     * @brief Abstract interface for interpolating values of type T.
     * @tparam T The type to interpolate.
     */
    template <typename T>
    struct IInterpolator {
        /** @brief Virtual destructor. */
        virtual ~IInterpolator() = default;

        /**
         * @brief Linearly interpolates between two values.
         * @param a The start value.
         * @param b The end value.
         * @param t The interpolation factor in [0, 1].
         * @param result The interpolated output.
         */
        virtual void Linear(const T& a, const T& b, double t, T& result) = 0;
        /**
         * @brief Barycentric interpolation across a triangle.
         * @param a The value at the first vertex.
         * @param b The value at the second vertex.
         * @param c The value at the third vertex.
         * @param wa Barycentric weight for vertex a.
         * @param wb Barycentric weight for vertex b.
         * @param wc Barycentric weight for vertex c.
         * @param result The interpolated output.
         */
        virtual void Triangle(const T& a, const T& b, const T& c, double wa, double wb, double wc, T& result) = 0;
    };

    /**
     * @brief Default interpolator that uses arithmetic operators on T for blending.
     *        Requires T to support scalar multiplication and addition.
     * @tparam T The type to interpolate.
     */
    template <typename T>
    struct Interpolator : public IInterpolator<T> {
        void Linear(const T& a, const T& b, double t, T& result) override {
            result = ((a * (1.0 - t)) + (b * t));
        }
        void Triangle(const T& a, const T& b, const T& c, double wa, double wb, double wc, T& result) override {
            result = ((a * wa) + (b * wb) + (c * wc));
        }
    };

    /**
     * @brief An IInterpolator implementation that delegates to user-provided functions.
     * @tparam T The type to interpolate.
     */
    template <typename T>
    class FunctionInterpolator : public IInterpolator<T> {
    private:
        std::function<void(const T&, const T&, double, T&)> linear_func;
        std::function<void(const T&, const T&, const T&, double, double, double, T&)> triangle_func;
    public:
        /**
         * @brief Constructor.
         * @param _linear_func The function to use for linear interpolation.
         * @param _triangle_func The function to use for triangle/barycentric interpolation.
         */
        FunctionInterpolator(
            const std::function<void(const T&, const T&, double, T&)>& _linear_func,
            const std::function<void(const T&, const T&, const T&, double, double, double, T&)>& _triangle_func)
            : linear_func(_linear_func), triangle_func(_triangle_func) {}
        
        /**
         * @brief Replaces the linear interpolation function.
         * @param func The new linear function.
         */
        void SetLinear(const std::function<void(const T&, const T&, double, T&)>& func) { linear_func = func; }
        /**
         * @brief Replaces the triangle interpolation function.
         * @param func The new triangle function.
         */
        void SetTriangle(const std::function<void(const T&, const T&, const T&, double, double, double, T&)>& func) { triangle_func = func; }

        void Linear(const T& a, const T& b, double t, T& result) override { linear_func(a, b, t, result); }
        void Triangle(const T& a, const T& b, const T& c, double wa, double wb, double wc, T& result) override { triangle_func(a, b, c, wa, wb, wc, result); }
    };

    /**
     * @brief Extension of IInterpolator that also supports scalar scaling of a value.
     * @tparam T The type to interpolate.
     */
    template <typename T>
    struct IScalableInterpolator : public IInterpolator<T> {
        /** @brief Virtual destructor. */
        virtual ~IScalableInterpolator() = default;

        /**
         * @brief Scales a value by a scalar factor.
         * @param in The input value.
         * @param scalar The factor to scale by.
         * @param result The scaled output.
         */
        virtual void Scale(const T& in, double scalar, T& result) = 0;
    };
    
    /**
     * @brief Default scalable interpolator that uses arithmetic operators on T.
     *        Requires T to support scalar multiplication and addition.
     * @tparam T The type to interpolate.
     */
    template <typename T>
    struct ScalableInterpolator : public IScalableInterpolator<T> {
        void Scale(const T& in, double scalar, T& result) override { result = in * scalar; }
        void Linear(const T& a, const T& b, double t, T& result) override {
            result = ((a * (1.0 - t)) + (b * t));
        }
        void Triangle(const T& a, const T& b, const T& c, double wa, double wb, double wc, T& result) override {
            result = ((a * wa) + (b * wb) + (c * wc));
        }
    };

    /**
     * @brief An IScalableInterpolator implementation that delegates all operations to user-provided functions.
     * @tparam T The type to interpolate.
     */
    template <typename T>
    class FunctionScalableInterpolator : public IScalableInterpolator<T> {
    private:
        std::function<void(const T&, double, T&)> scale_func;
        std::function<void(const T&, const T&, double, T&)> linear_func;
        std::function<void(const T&, const T&, const T&, double, double, double, T&)> triangle_func;
    public:
        /**
         * @brief Constructor.
         * @param _scale_func The function to use for scaling.
         * @param _linear_func The function to use for linear interpolation.
         * @param _triangle_func The function to use for triangle/barycentric interpolation.
         */
        FunctionScalableInterpolator(
            const std::function<void(const T&, double, T&)>& _scale_func,
            const std::function<void(const T&, const T&, double, T&)>& _linear_func,
            const std::function<void(const T&, const T&, const T&, double, double, double, T&)>& _triangle_func)
            : scale_func(_scale_func), linear_func(_linear_func), triangle_func(_triangle_func) {}
        
        /**
         * @brief Replaces the scale function.
         * @param func The new scale function.
         */
        void SetScale(const std::function<void(const T&, double, T&)>& func) { scale_func = func; }
        /**
         * @brief Replaces the linear interpolation function.
         * @param func The new linear function.
         */
        void SetLinear(const std::function<void(const T&, const T&, double, T&)>& func) { linear_func = func; }
        /**
         * @brief Replaces the triangle interpolation function.
         * @param func The new triangle function.
         */
        void SetTriangle(const std::function<void(const T&, const T&, const T&, double, double, double, T&)>& func) { triangle_func = func; }

        void Scale(const T& in, double scalar, T& result) override { scale_func(in, scalar, result); }
        void Linear(const T& a, const T& b, double t, T& result) override { linear_func(a, b, t, result); }
        void Triangle(const T& a, const T& b, const T& c, double wa, double wb, double wc, T& result) override { triangle_func(a, b, c, wa, wb, wc, result); }
    };

}