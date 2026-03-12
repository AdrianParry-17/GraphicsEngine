#pragma once

#include "Engine_Vector.h"
#include "Engine_Constant.h"

#include <array>
#include <numeric>
#include <algorithm>

namespace Engine {
    /**
     * @brief A generic N-dimensional square matrix class.
     * @tparam D The dimensionality of the matrix (number of rows and columns).
     */
    template <size_t D>
    struct SquareMatrix {
    public:
        /**
         * @brief The dimensionality of the matrix.
         */
        static const size_t Dimension = D;
        /**
         * @brief The total number of cells/elements in the matrix.
         */
        static const size_t CellCount = D * D;
	private:
		std::array<double, CellCount> data;
	public:
		/**
		 * @brief Default constructor, initializes all elements to zero.
		 */
		SquareMatrix() { data.fill(0); }
		/**
		 * @brief Constructor that fills all elements with the given value.
		 * @param _fill The value to fill all elements with.
		 */
		SquareMatrix(double _fill) { data.fill(_fill); }
		/**
		 * @brief Constructor from a standard array.
		 * @param _data The array containing the matrix data in row-major order.
		 */
		SquareMatrix(const std::array<double, CellCount>& _data) : data(_data) {}

		SquareMatrix<D> operator-() const {
			std::array<double, CellCount> _tmp;
			for (int i = 0; i < CellCount; ++i) _tmp[i] = -data[i];
			return SquareMatrix<D>(_tmp);
		}
		double& operator()(int r, int c) { return data[r * D + c]; }
		const double& operator()(int r, int c) const { return data[r * D + c]; }
		
		SquareMatrix<D>& operator=(const SquareMatrix<D>& _m) { data = _m.data; return *this; }
		SquareMatrix<D>& operator=(const std::array<double, CellCount>& _m) { data = _m; return *this; }
		SquareMatrix<D>& operator=(double _fill) { data.fill(_fill); return *this; }
		SquareMatrix<D>& operator+=(const std::array<double, CellCount>& _m) { SquareMatrix<D>::Add(data, _m, data); return *this; }
		SquareMatrix<D>& operator+=(const SquareMatrix<D>& _m) { SquareMatrix<D>::Add(data, _m.data, data); return *this; }
		SquareMatrix<D>& operator+=(double _v) {
			for (double& v : data) v += _v;
			return *this;
		}
		SquareMatrix<D>& operator-=(const std::array<double, CellCount>& _m) { SquareMatrix<D>::Subtract(data, _m, data); return *this; }
		SquareMatrix<D>& operator-=(const SquareMatrix<D>& _m) { SquareMatrix<D>::Subtract(data, _m.data, data); return *this; }
		SquareMatrix<D>& operator-=(double _v) {
			for (double& v : data) v -= _v;
			return *this;
		}

		SquareMatrix<D>& operator*=(double _s) {
			for (double& v : data) v *= _s;
			return *this;
		}
		SquareMatrix<D>& operator*=(const std::array<double, CellCount>& _m) {
			std::array<double, CellCount> _tmp;
			SquareMatrix<D>::Multiply(data, _m, _tmp);
			data.swap(_tmp);
			return *this;
		}
		SquareMatrix<D>& operator*=(const SquareMatrix<D>& _m) {
			std::array<double, CellCount> _tmp;
			SquareMatrix<D>::Multiply(data, _m.data, _tmp);
			data.swap(_tmp);
			return *this;
		}

		SquareMatrix<D> operator+(const SquareMatrix<D>& _m) const {
			std::array<double, CellCount> _tmp;
			SquareMatrix<D>::Add(data, _m.data, _tmp);
			return SquareMatrix<D>(_tmp);
		}
		SquareMatrix<D> operator+(const std::array<double, CellCount>& _m) const {
			std::array<double, CellCount> _tmp;
			SquareMatrix<D>::Add(data, _m, _tmp);
			return SquareMatrix<D>(_tmp);
		}
		SquareMatrix<D> operator+(double _v) const {
			std::array<double, CellCount> _tmp;
			for (int i = 0; i < CellCount; ++i) _tmp[i] = data[i] + _v;
			return SquareMatrix<D>(_tmp);
		}
		friend SquareMatrix<D> operator+(const std::array<double, CellCount>& l, const SquareMatrix<D>& r) {
			std::array<double, CellCount> _tmp;
			SquareMatrix<D>::Add(l, r.data, _tmp);
			return SquareMatrix<D>(_tmp);
		}
		friend SquareMatrix<D> operator+(double _v, const SquareMatrix<D>& r) {
			std::array<double, CellCount> _tmp;
			for (int i = 0; i < CellCount; ++i) _tmp[i] = _v + r.data[i];
			return SquareMatrix<D>(_tmp);
		}

		SquareMatrix<D> operator-(const SquareMatrix<D>& _m) const {
			std::array<double, CellCount> _tmp;
			SquareMatrix<D>::Subtract(data, _m.data, _tmp);
			return SquareMatrix<D>(_tmp);
		}
		SquareMatrix<D> operator-(const std::array<double, CellCount>& _m) const {
			std::array<double, CellCount> _tmp;
			SquareMatrix<D>::Subtract(data, _m, _tmp);
			return SquareMatrix<D>(_tmp);
		}
		SquareMatrix<D> operator-(double _v) const {
			std::array<double, CellCount> _tmp;
			for (int i = 0; i < CellCount; ++i) _tmp[i] = data[i] - _v;
			return SquareMatrix<D>(_tmp);
		}
		friend SquareMatrix<D> operator-(const std::array<double, CellCount>& l, const SquareMatrix<D>& r) {
			std::array<double, CellCount> _tmp;
			SquareMatrix<D>::Subtract(l, r.data, _tmp);
			return SquareMatrix<D>(_tmp);
		}
		friend SquareMatrix<D> operator-(double _v, const SquareMatrix<D>& r) {
			std::array<double, CellCount> _tmp;
			for (int i = 0; i < CellCount; ++i) _tmp[i] = _v - r.data[i];
			return SquareMatrix<D>(_tmp);
		}

		SquareMatrix<D> operator*(const SquareMatrix<D>& _m) const {
			std::array<double, CellCount> _tmp;
			SquareMatrix<D>::Multiply(data, _m.data, _tmp);
			return SquareMatrix<D>(_tmp);
		}
		SquareMatrix<D> operator*(const std::array<double, CellCount>& _m) const {
			std::array<double, CellCount> _tmp;
			SquareMatrix<D>::Multiply(data, _m, _tmp);
			return SquareMatrix<D>(_tmp);
		}
		SquareMatrix<D> operator*(double _v) const {
			std::array<double, CellCount> _tmp;
			for (int i = 0; i < CellCount; ++i) _tmp[i] = data[i] * _v;
			return SquareMatrix<D>(_tmp);
		}
		friend SquareMatrix<D> operator*(const std::array<double, CellCount>& l, const SquareMatrix<D>& r) {
			std::array<double, CellCount> _tmp;
			SquareMatrix<D>::Multiply(l, r.data, _tmp);
			return SquareMatrix<D>(_tmp);
		}
		friend SquareMatrix<D> operator*(double _v, const SquareMatrix<D>& r) {
			std::array<double, CellCount> _tmp;
			for (int i = 0; i < CellCount; ++i) _tmp[i] = _v * r.data[i];
			return SquareMatrix<D>(_tmp);
		}

		Vector<D> operator*(const Vector<D>& _v) const {
            std::array<double, D> res;
            for (size_t r = 0; r < D; ++r)
                res[r] = std::inner_product(data.begin() + (r * D), data.begin() + ((r + 1) * D), _v.begin(), 0.0);
            return res;
		}

		double& operator[](int i) { return data[i]; }
		const double& operator[](int i) const { return data[i]; }

		/**
		 * @brief Get the underlying data array.
		 * @return A reference to the underlying std::array.
		 */
		std::array<double, CellCount>& Data() { return data; }
		/**
		 * @brief Get the underlying data array as const.
		 * @return A const reference to the underlying std::array.
		 */
		const std::array<double, CellCount>& Data() const { return data; }

		/**
		 * @brief Static math function to add two arrays element-wise.
		 */
		static void Add(const std::array<double, CellCount>& l, const std::array<double, CellCount>& r, std::array<double, CellCount>& res) {
			std::transform(l.cbegin(), l.cend(), r.cbegin(), res.begin(), std::plus<double>());
		}
		/**
		 * @brief Static math function to add two matrices.
		 */
		static void Add(const SquareMatrix<D>& l, const SquareMatrix<D>& r, SquareMatrix<D>& res) { SquareMatrix<D>::Add(l.data, r.data, res.data); }

		/**
		 * @brief Static math function to subtract two arrays element-wise.
		 */
		static void Subtract(const std::array<double, CellCount>& l, const std::array<double, CellCount>& r, std::array<double, CellCount>& res) {
			std::transform(l.cbegin(), l.cend(), r.cbegin(), res.begin(), std::minus<double>());
		}
		/**
		 * @brief Static math function to subtract two matrices.
		 */
		static void Subtract(const SquareMatrix<D>& l, const SquareMatrix<D>& r, SquareMatrix<D>& res) { SquareMatrix<D>::Subtract(l.data, r.data, res.data); }

		/**
		 * @brief Static math function to multiply two arrays (matrix multiplication).
		 */
		static void Multiply(const std::array<double, CellCount>& l, const std::array<double, CellCount>& r, std::array<double, CellCount>& res) {
			auto res_ptr = res.begin();
			for (int _r = 0; _r < D; ++_r) {
				for (int _c = 0; _c < D; ++_c, ++res_ptr) {
                    (*res_ptr) = 0;
                    for (int i = 0; i < D; ++i)
                        (*res_ptr) += l[_r * D + i] * r[_c + (D * i)];
                }
			}
		}
		/**
		 * @brief Static math function to multiply two matrices.
		 */
		static void Multiply(const SquareMatrix<D>& l, const SquareMatrix<D>& r, SquareMatrix<D>& res) { SquareMatrix<D>::Multiply(l.data, r.data, res.data); }

		/**
		 * @brief Generates an identity matrix into an array.
		 * @param res The array to store the result in.
		 * @param scalar The value for the diagonal elements.
		 */
		static void GetIdentity(std::array<double, CellCount>& res, double scalar = 1.0) {
            for (size_t r = 0; r < D; ++r) {
                size_t idx = r * D;
                if (r > 0) std::fill_n(res.begin() + idx, r, 0.0);
                res[idx + r] = scalar;
                if (r < (D - 1)) std::fill_n(res.begin() + (idx + r + 1), D - (r + 1), 0.0);
            }
		}
		/**
		 * @brief Generates an identity matrix into another matrix object.
		 * @param res The matrix to store the result in.
		 * @param scalar The value for the diagonal elements.
		 */
		static void GetIdentity(SquareMatrix<D>& res, double scalar = 1.0) { SquareMatrix<D>::GetIdentity(res.data, scalar); }
		/**
		 * @brief Generates and returns an identity matrix.
		 * @param scalar The value for the diagonal elements.
		 * @return The generated identity matrix.
		 */
		static SquareMatrix<D> GetIdentity(double scalar = 1.0) {
			std::array<double, CellCount> _tmp;
			SquareMatrix<D>::GetIdentity(_tmp, scalar);
			return SquareMatrix<D>(_tmp);
		}
    };

    /**
     * @brief 2x2 Square Matrix
     */
    typedef SquareMatrix<2> Matrix2x2;
    /**
     * @brief 3x3 Square Matrix
     */
    typedef SquareMatrix<3> Matrix3x3;
    /**
     * @brief 4x4 Square Matrix
     */
    typedef SquareMatrix<4> Matrix4x4;
}