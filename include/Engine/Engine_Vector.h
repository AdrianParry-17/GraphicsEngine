#pragma once

#include "Engine_Constant.h"

#include <array>
#include <math.h>
#include <algorithm>
#include <numeric>
#include <type_traits>
#include <functional>

namespace Engine {
    /**
     * @brief A generic N-dimensional vector class.
     * @tparam D The dimensionality of the vector.
     */
    template <size_t D>
    struct Vector {
    private:
        std::array<double, D> data;
    public:
        /**
         * @brief The dimensionality of the vector.
         */
        static const size_t Dimension = D;

        /**
         * @brief Default constructor, initializes all elements to zero.
         */
        Vector() { data.fill(0); }
        /**
         * @brief Constructor that fills all elements with the given value.
         * @param _fill The value to fill all elements with.
         */
        Vector(double _fill) { data.fill(_fill); }
        /**
         * @brief Constructor from a standard array.
         * @param _data The array to initialize the vector from.
         */
        Vector(const std::array<double, D>& _data) : data(_data) {}
        /**
         * @brief Constructor from a standard array of different dimensionality.
         * @tparam D2 The dimensionality of the input array.
         * @param _data The array to initialize the vector from.
         */
        template <size_t D2> Vector(const std::array<double, D2>& _data) { Assign<D2>(_data); }
        /**
         * @brief Constructor from a vector of different dimensionality.
         * @tparam D2 The dimensionality of the input vector.
         * @param _data The vector to initialize from.
         */
        template <size_t D2> Vector(const Vector<D2>& _data) { Assign<D2>(_data.Data()); }
        /**
         * @brief Constructor for 2D vectors.
         * @param x The x component.
         * @param y The y component.
         */
            
        template <size_t _D = D>
        Vector(typename std::enable_if<(_D == D) && (D >= 2), double>::type x, double y) { data.fill(0); data[0] = x; data[1] = y; }
        /**
         * @brief Constructor for 3D vectors.
         * @param x The x component.
         * @param y The y component.
         * @param z The z component.
         */
        template <size_t _D = D>
        Vector(typename std::enable_if<(_D == D) && (D >= 3), double>::type x, double y, double z) { data.fill(0); data[0] = x; data[1] = y; data[2] = z; }
        /**
         * @brief Constructor for 4D vectors.
         * @param x The x component.
         * @param y The y component.
         * @param z The z component.
         * @param w The w component.
         */
        template <size_t _D = D>
        Vector(typename std::enable_if<(_D == D) && (D >= 4), double>::type x, double y, double z, double w) { data.fill(0); data[0] = x; data[1] = y; data[2] = z; data[3] = w; }

        Vector<D>& operator=(double _fill) { Assign(_fill); return *this; }
        Vector<D>& operator=(const std::array<double, D>& _data) { Assign(_data); return *this; }
        Vector<D>& operator=(const Vector<D>& _v) { Assign(_v.data); return *this; }
        template <size_t D2>
        Vector<D>& operator=(const std::array<double, D2>& _data) { Assign<D2>(_data); return *this; }
        template <size_t D2>
        Vector<D>& operator=(const Vector<D2>& _data) { Assign<D2>(_data); return *this; }

        Vector<D>& operator+=(double _fill) { Vector<D>::Add(data, _fill, data); return *this; }
        Vector<D>& operator+=(const std::array<double, D>& _data) { Vector<D>::Add(data, _data, data); return *this; }
        Vector<D>& operator+=(const Vector<D>& _v) { Vector<D>::Add(data, _v.data, data); return *this; }
        template <size_t D2>
        Vector<D>& operator+=(const std::array<double, D2>& _data) { Vector<D>::Add<D2>(data, _data, data); return *this; }
        template <size_t D2>
        Vector<D>& operator+=(const Vector<D2>& _data) { Vector<D>::Add<D2>(data, _data.Data(), data); return *this; }

        Vector<D>& operator-=(double _fill) { Vector<D>::Subtract(data, _fill, data); return *this; }
        Vector<D>& operator-=(const std::array<double, D>& _data) { Vector<D>::Subtract(data, _data, data); return *this; }
        Vector<D>& operator-=(const Vector<D>& _v) { Vector<D>::Subtract(data, _v.data, data); return *this; }
        template <size_t D2>
        Vector<D>& operator-=(const std::array<double, D2>& _data) { Vector<D>::Subtract<D2>(data, _data, data); return *this; }
        template <size_t D2>
        Vector<D>& operator-=(const Vector<D2>& _data) { Vector<D>::Subtract<D2>(data, _data.Data(), data); return *this; }

        Vector<D>& operator*=(double _s) { Vector<D>::Multiply(data, _s, data); return *this; } 

        Vector<D> operator+(const std::array<double, D>& _data) const {
            std::array<double, D> tmp; Vector<D>::Add(data, _data, tmp); return tmp;
        }
        Vector<D> operator+(const Vector<D>& _data) const {
            std::array<double, D> tmp; Vector<D>::Add(data, _data.data, tmp); return tmp;
        }
        Vector<D> operator+(double _fill) const {
            std::array<double, D> tmp; Vector<D>::Add(data, _fill, tmp); return tmp;
        }
        friend Vector<D> operator+(double _fill, const Vector<D>& _data) {
            std::array<double, D> tmp; Vector<D>::Add(_data.data, _fill, tmp); return tmp;
        }
        template <size_t D2>
        Vector<D> operator+(const std::array<double, D2>& _data) const {
            std::array<double, D> tmp; Vector<D>::Add<D2>(data, _data, tmp); return tmp;
        }
        template <size_t D2>
        Vector<D> operator+(const Vector<D2>& _data) const {
            std::array<double, D> tmp; Vector<D>::Add<D2>(data, _data.Data(), tmp); return tmp;
        }

        Vector<D> operator-(const std::array<double, D>& _data) const {
            std::array<double, D> tmp; Vector<D>::Subtract(data, _data, tmp); return tmp;
        }
        Vector<D> operator-(const Vector<D>& _data) const {
            std::array<double, D> tmp; Vector<D>::Subtract(data, _data.data, tmp); return tmp;
        }
        Vector<D> operator-(double _fill) const {
            std::array<double, D> tmp; Vector<D>::Subtract(data, _fill, tmp); return tmp;
        }
        friend Vector<D> operator-(double _fill, const Vector<D>& _data) {
            std::array<double, D> tmp; Vector<D>::Subtract(_fill, _data.data, tmp); return tmp;
        }
        template <size_t D2>
        Vector<D> operator-(const std::array<double, D2>& _data) const {
            std::array<double, D> tmp; Vector<D>::Subtract<D2>(data, _data, tmp); return tmp;
        }
        template <size_t D2>
        Vector<D> operator-(const Vector<D2>& _data) const {
            std::array<double, D> tmp; Vector<D>::Subtract<D2>(data, _data.Data(), tmp); return tmp;
        }

        Vector<D> operator*(double _s) const {
            std::array<double, D> tmp; Vector<D>::Multiply(data, _s, tmp); return tmp;
        }
        friend Vector<D> operator*(double _s, const Vector<D>& _data) {
            std::array<double, D> tmp; Vector<D>::Multiply(_data.data, _s, tmp); return tmp;
        }

        Vector<D> operator-() const {
            std::array<double, D> res;
            auto rp = res.begin();
            for (auto p = data.cbegin(); p != data.cend(); ++p, ++rp) (*rp) = -(*p);
            return res;
        }

        bool operator==(const std::array<double, D>& _data) const { return data == _data; }
        bool operator==(const Vector<D>& _data) const { return operator==(_data.data); }
        bool operator==(double _fill) const { return std::all_of(data.begin(), data.end(), [&_fill](const double& v) { return _fill == v; }); }
        friend bool operator==(double _fill, const Vector<D>& _v) { return _v == _fill; }
        template <size_t D2>
        bool operator==(const std::array<double, D2>& _data) const {
            if (!std::equal(data.begin(), data.begin() + std::min(D, D2), _data.begin())) return false;
            return (D <= D2 || std::all_of(data.begin() + D2, data.end(), [](const double& v) { return v == 0.0; }));
        }
        template <size_t D2>
        Vector<D> operator==(const Vector<D2>& _data) const { return operator==<D2>(_data.Data()); }

        bool operator!=(const std::array<double, D>& _data) const { return !operator==(_data); }
        bool operator!=(const Vector<D>& _data) const { return !operator==(_data.data); }
        bool operator!=(double _fill) const { return !operator==(_fill); }
        friend bool operator!=(double _fill, const Vector<D>& _v) { return _v != _fill; }
        template <size_t D2>
        bool operator!=(const std::array<double, D2>& _data) const { return !operator==<D2>(_data); }
        template <size_t D2>
        Vector<D> operator!=(const Vector<D2>& _data) const { return operator!=<D2>(_data.Data()); }

        double& operator[](size_t d) { return data[d]; }
        const double& operator[](size_t d) const { return data[d]; }

        /**
         * @brief Get the underlying data array.
         * @return A reference to the underlying std::array.
         */
        std::array<double, D>& Data() { return data; }
        /**
         * @brief Get the underlying data array as const.
         * @return A const reference to the underlying std::array.
         */
        const std::array<double, D>& Data() const { return data; }

        /**
         * @brief Access the x component.
         * @return A reference to the x component.
         */
        template <size_t _D = D>
        typename std::enable_if<(_D == D) && (D >= 1), double&>::type x() { return operator[](0); }
        /**
         * @brief Access the x component as const.
         * @return A const reference to the x component.
         */
        template <size_t _D = D>
        typename std::enable_if<(_D == D) && (D >= 1), const double&>::type x() const { return operator[](0); }
        /**
         * @brief Access the y component.
         * @return A reference to the y component.
         */
        template <size_t _D = D>
        typename std::enable_if<(_D == D) && (D >= 2), double&>::type y() { return operator[](1); }
        /**
         * @brief Access the y component as const.
         * @return A const reference to the y component.
         */
        template <size_t _D = D>
        typename std::enable_if<(_D == D) && (D >= 2), const double&>::type y() const { return operator[](1); }
        /**
         * @brief Access the z component.
         * @return A reference to the z component.
         */
        template <size_t _D = D>
        typename std::enable_if<(_D == D) && (D >= 3), double&>::type z() { return operator[](2); }
        /**
         * @brief Access the z component as const.
         * @return A const reference to the z component.
         */
        template <size_t _D = D>
        typename std::enable_if<(_D == D) && (D >= 3), const double&>::type z() const { return operator[](2); }
        /**
         * @brief Access the w component.
         * @return A reference to the w component.
         */
        template <size_t _D = D>
        typename std::enable_if<(_D == D) && (D >= 4), double&>::type w() { return operator[](3); }
        /**
         * @brief Access the w component as const.
         * @return A const reference to the w component.
         */
        template <size_t _D = D>
        typename std::enable_if<(_D == D) && (D >= 4), const double&>::type w() const { return operator[](3); }

        /**
         * @brief Calculates the sum of all elements in the vector.
         * @return The sum of all elements.
         */
        double Total() const { return std::accumulate(data.begin(), data.end(), 0.0); }

        /**
         * @brief Checks if the vector is identical to an array within a small tolerance.
         * @param _data The array to compare against.
         * @return True if identical, false otherwise.
         */
        bool IsIdentical(const std::array<double, D>& _data) const {
            auto rp = _data.begin();
            for (auto lp = data.begin(); lp != data.end(); ++lp, ++rp)
                if (!NumericConstants::IsNearZero((*lp) - (*rp))) return false;

            return true;
        }
        /**
         * @brief Checks if the vector is identical to another vector within a small tolerance.
         * @param _data The vector to compare against.
         * @return True if identical, false otherwise.
         */
        bool IsIdentical(const Vector<D>& _data) const { return IsIdentical(_data.data); }
        /**
         * @brief Checks if all elements of the vector are identical to a value within a small tolerance.
         * @param _fill The value to compare against.
         * @return True if identical, false otherwise.
         */
        bool IsIdentical(double _fill) const {
            return std::all_of(data.begin(), data.end(), [&_fill](const double& v) { return NumericConstants::IsNearZero(v - _fill); });
        }
        /**
         * @brief Checks if the vector is identical to an array of different dimensionality within a small tolerance.
         * @tparam D2 The dimensionality of the input array.
         * @param _data The array to compare against.
         * @return True if identical, false otherwise.
         */
        template <size_t D2>
        bool IsIdentical(const std::array<double, D2>& _data) const {
            if (!std::equal(data.begin(), data.begin() + std::min(D, D2), _data.begin(), [](const double& l, const double& r) { return NumericConstants::IsNearZero(l-r); }))
                return false;
            return (D <= D2 || std::all_of(data.begin() + D2, data.end(), [](const double& v) { return NumericConstants::IsNearZero(v); }));
        }
        /**
         * @brief Checks if the vector is identical to an vector of different dimensionality within a small tolerance.
         * @tparam D2 The dimensionality of the input vector.
         * @param _data The vector to compare against.
         * @return True if identical, false otherwise.
         */
        template <size_t D2>
        Vector<D> IsIdentical(const Vector<D2>& _data) const { return IsIdentical<D2>(_data.Data()); }


        /**
         * @brief Assigns a single value to all elements of the vector.
         * @param _fill The value to assign.
         */
        void Assign(double _fill) { data.fill(_fill); }
        /**
         * @brief Assigns an array to the vector.
         * @param _data The array to assign.
         */
        void Assign(const std::array<double, D>& _data) { data = _data; }
        /**
         * @brief Assigns a vector to the vector.
         * @param _v The vector to assign.
         */
        void Assign(const Vector<D>& _v) { Assign(_v.data); }
        /**
         * @brief Assigns an array of different dimensionality to the vector.
         * @tparam D2 The dimensionality of the input array.
         * @param _data The array to assign.
         */
        template <size_t D2> void Assign(const std::array<double, D2>& _data) {
            auto ptr = std::copy(_data.begin(), _data.begin() + std::min(D, D2), data.begin());
            if (ptr != data.end()) std::fill(ptr, data.end(), 0.0); 
        }
        /**
         * @brief Assigns a vector of different dimensionality to the vector.
         * @tparam D2 The dimensionality of the input vector.
         * @param _data The vector to assign.
         */
        template <size_t D2> void Assign(const Vector<D2>& _data) { Assign<D2>(_data.Data()); }
        
        /**
         * @brief Calculates the magnitude (length) of the vector.
         * @return The magnitude of the vector.
         */
        double Magnitude() const {
            return std::accumulate(data.begin(), data.end(), 0.0, std::hypot);
        }
        /**
         * @brief Returns a normalized copy of the vector.
         * @return The normalized vector.
         */
        Vector<D> GetNormalized() const {
            double factor = Magnitude();
            if (NumericConstants::IsNearZero(factor))
                factor = NumericConstants::NearZero;
            factor = 1.0 / factor;
            
            std::array<double, D> tmp;
            auto rp = tmp.begin();
            for (auto p = data.begin(); p != data.end(); ++p, ++rp) (*rp) = (*p) * factor;
            return tmp;
        }
        /**
         * @brief Normalizes the vector in-place.
         */
        void Normalize() {
            double factor = Magnitude();
            if (NumericConstants::IsNearZero(factor))
                factor = NumericConstants::NearZero;
            factor = 1.0 / factor;

            for (double& v : data) v *= factor;
        }

        /**
         * @brief Calculates the dot product of this vector with an array.
         * @param _data The array to calculate the dot product with.
         * @return The dot product.
         */
        double DotProductWith(const std::array<double, D>& _data) const { return Vector<D>::DotProduct(data, _data);} 
        /**
         * @brief Calculates the dot product of this vector with another vector.
         * @param _data The vector to calculate the dot product with.
         * @return The dot product.
         */
        double DotProductWith(const Vector<D>& _data) const { return Vector<D>::DotProduct(*this, _data); }
        /**
         * @brief Calculates the dot product as if all elements of the other vector were _fill.
         * @param _fill The value to calculate the dot product with.
         * @return The dot product.
         */
        double DotProductWith(double _fill) const { return Total() * _fill; }
        /**
         * @brief Calculates the dot product with an array of different dimensionality.
         * @tparam D2 The dimensionality of the input array.
         * @param _data The array to calculate the dot product with.
         * @return The dot product.
         */
        template <size_t D2>
        double DotProductWith(const std::array<double, D2>& _data) const { return Vector<D>::DotProduct<D2>(data, _data);} 
        /**
         * @brief Calculates the dot product with a vector of different dimensionality.
         * @tparam D2 The dimensionality of the input vector.
         * @param _data The vector to calculate the dot product with.
         * @return The dot product.
         */
        template <size_t D2>
        double DotProductWith(const Vector<D2>& _data) const { return Vector<D>::DotProduct<D2>(*this, _data); }

        typename std::array<double, D>::iterator begin() { return data.begin(); }
        typename std::array<double, D>::const_iterator begin() const { return data.begin(); }
        typename std::array<double, D>::iterator end() { return data.end(); }
        typename std::array<double, D>::const_iterator end() const { return data.end(); }
        typename std::array<double, D>::const_iterator cbegin() const { return data.cbegin(); }
        typename std::array<double, D>::const_iterator cend() const { return data.cend(); }
        typename std::array<double, D>::reverse_iterator rbegin() { return data.rbegin(); }
        typename std::array<double, D>::const_reverse_iterator rbegin() const { return data.rbegin(); }
        typename std::array<double, D>::reverse_iterator rend() { return data.rend(); }
        typename std::array<double, D>::const_reverse_iterator rend() const { return data.rend(); }

        /**
         * @brief Static math function to add a scalar to an array element-wise.
         */
        static void Add(const std::array<double, D>& l, double r, std::array<double, D>& res) {
            auto res_p = res.begin();
            for (auto lp = l.cbegin(); lp != l.end(); ++lp, ++res_p) (*res_p) = (*lp) + r;
        }
        /**
         * @brief Static math function to add two arrays element-wise.
         */
        static void Add(const std::array<double, D>& l, const std::array<double, D>& r, std::array<double, D>& res) {
            std::transform(l.begin(), l.end(), r.begin(), res.begin(), std::plus<double>());
        }
        /**
         * @brief Static math function to add two arrays of different dimension element-wise.
         */
        template <size_t D2>
        static void Add(const std::array<double, D>& l, const std::array<double, D2>& r, std::array<double, D>& res) {
            std::transform(l.begin(), l.begin() + std::min(D, D2), r.begin(), res.begin(), std::plus<double>());
        }
        /**
         * @brief Static math function to subtract a scalar from an array element-wise.
         */
        static void Subtract(const std::array<double, D>& l, double r, std::array<double, D>& res) {
            auto res_p = res.begin();
            for (auto lp = l.cbegin(); lp != l.end(); ++lp, ++res_p) (*res_p) = (*lp) - r;
        }
        /**
         * @brief Static math function to subtract an array from a scalar element-wise.
         */
        static void Subtract(double l, const std::array<double, D>& r, std::array<double, D>& res) {
            auto res_p = res.begin();
            for (auto rp = r.cbegin(); rp != r.end(); ++rp, ++res_p) (*res_p) = l - (*rp);
        }
        /**
         * @brief Static math function to subtract two arrays element-wise.
         */
        static void Subtract(const std::array<double, D>& l, const std::array<double, D>& r, std::array<double, D>& res) {
            std::transform(l.begin(), l.end(), r.begin(), res.begin(), std::minus<double>());
        }
        /**
         * @brief Static math function to subtract two arrays of different dimension element-wise.
         */
        template <size_t D2>
        static void Subtract(const std::array<double, D>& l, const std::array<double, D2>& r, std::array<double, D>& res) {
            std::transform(l.begin(), l.begin() + std::min(D, D2), r.begin(), res.begin(), std::minus<double>());
        }
        /**
         * @brief Static math function to multiply an array and a scalar element-wise.
         */
        static void Multiply(const std::array<double, D>& l, double r, std::array<double, D>& res) {
            auto res_p = res.begin();
            for (auto lp = l.cbegin(); lp != l.end(); ++lp, ++res_p) (*res_p) = (*lp) * r;
        }

        /**
         * @brief Static math function to compute dot product of two arrays.
         */
        static double DotProduct(const std::array<double, D>& l, const std::array<double, D>& r) {
            return std::inner_product(l.begin(), l.end(), r.begin(), 0.0);
        }
        /**
         * @brief Static math function to compute dot product of two vectors.
         */
        static double DotProduct(const Vector<D>& l, const Vector<D>& r) { return Vector<D>::DotProduct(l.data, r.data); }
        /**
         * @brief Static math function to compute dot product of two arrays of different dimensions.
         */
        template <size_t D2>
        static double DotProduct(const std::array<double, D>& l, const std::array<double, D2>& r) {
            return std::inner_product(l.begin(), l.begin() + std::min(D, D2), r.begin(), 0.0);
        }
        /**
         * @brief Static math function to compute dot product of two vectors of different dimensions.
         */
        template <size_t D2>
        static double DotProduct(const Vector<D>& l, const Vector<D2>& r) { return Vector<D>::DotProduct<D2>(l.data, r.data); }

        static void Lerp(const std::array<double, D>& a, const std::array<double, D>& b, double t, std::array<double, D>& res) {
            std::transform(a.begin(), a.end(), b.begin(), res.begin(), [&t](const double& _a, const double& _b) { return _a + t * (_b - _a); });
        }
        static void Lerp(const Vector<D>& a, const Vector<D>& b, double t, Vector<D>& res) { return Vector<D>::Lerp(a.data, b.data, t, res.data); }
    };

    /**
     * @brief Two-dimensional vector.
     */
    typedef Vector<2> Vector2;
    /**
     * @brief Three-dimensional vector.
     */
    typedef Vector<3> Vector3;
    /**
     * @brief Four-dimensional vector.
     */
    typedef Vector<4> Vector4;

    /**
     * @brief Cross-product for three-dimensional vector.
     */
    inline Vector3 operator*(const Vector3& l, const Vector3& r) {
        std::array<double, 3> tmp;
        tmp[0] = l[1] * r[2] - l[2] * r[1];
        tmp[1] = l[2] * r[0] - l[0] * r[2];
        tmp[2] = l[0] * r[1] - l[1] * r[0];
        return tmp;
    }
    /**
     * @brief Cross-product for three-dimensional vector.
     */
    inline Vector3& operator*=(Vector3& l, const Vector3& r) {
        double _x = l[1] * r[2] - l[2] * r[1];
        double _y = l[2] * r[0] - l[0] * r[2];
        double _z = l[0] * r[1] - l[1] * r[0];
        l[0] = _x; l[1] = _y; l[2] = _z;
        return l;
    }
}