#pragma once

#include "Engine_Vector.h"
#include "Engine_Constant.h"

namespace Engine {
	/// @brief Quaternion utility type for 3D rotations and algebra.
	struct Quaternion {
	public:
		/// @brief Real component.
		double a = 0, b = 0, c = 0, d = 0;
		
		Quaternion() = default;
		Quaternion(double _a) : a(_a) {}
		Quaternion(double _a, double _b, double _c, double _d) : a(_a), b(_b), c(_c), d(_d) {}
		Quaternion(double _a, const Vector3& _img) : a(_a), b(_img.x), c(_img.y), d(_img.z) {}

		Quaternion operator-() const { return Quaternion(-a, -b, -c, -d); }
		
		Quaternion& operator=(const Quaternion& _q) { a = _q.a; b = _q.b; c = _q.c; d = _q.d; return *this; }
		Quaternion& operator=(double _a) { a = _a; b = 0; c = 0; d = 0; return *this; }
		Quaternion& operator+=(const Quaternion& _q) { a += _q.a; b += _q.b; c += _q.c; d += _q.d; return *this; }
		Quaternion& operator+=(double _a) { a += _a; return *this; }
		Quaternion& operator-=(const Quaternion& _q) { a -= _q.a; b -= _q.b; c -= _q.c; d -= _q.d; return *this; }
		Quaternion& operator-=(double _a) { a -= _a; return *this; }
		Quaternion& operator*=(const Quaternion& _q) {
			double _a = a * _q.a - b * _q.b - c * _q.c - d * _q.d;
			double _b = a * _q.b + b * _q.a + c * _q.d - d * _q.c;
			double _c = a * _q.c - b * _q.d + c * _q.a + d * _q.b;
			double _d = a * _q.d + b * _q.c - c * _q.b + d * _q.a;

			a = _a; b = _b; c = _c; d = _d;
			return *this;
		}
		Quaternion& operator*=(double _a) { a *= _a; b *= _a; c *= _a; d *= _a; return *this; }

		Quaternion operator+(const Quaternion& _q) const { return Quaternion(a + _q.a, b + _q.b, c + _q.c, d + _q.d); }
		Quaternion operator+(double _a) const { return Quaternion(a + _a, b, c, d); }
		friend Quaternion operator+(double _a, const Quaternion& _q) { return Quaternion(_a + _q.a, _q.b, _q.c, _q.d); }
		Quaternion operator-(const Quaternion& _q) const { return Quaternion(a - _q.a, b - _q.b, c - _q.c, d - _q.d); }
		Quaternion operator-(double _a) const { return Quaternion(a - _a, b, c, d); }
		friend Quaternion operator-(double _a, const Quaternion& _q) { return Quaternion(_a - _q.a, -_q.b, -_q.c, -_q.d); }
		Quaternion operator*(const Quaternion& _q) const {
			return Quaternion(
				a * _q.a - b * _q.b - c * _q.c - d * _q.d,
				a * _q.b + b * _q.a + c * _q.d - d * _q.c,
				a * _q.c - b * _q.d + c * _q.a + d * _q.b,
				a * _q.d + b * _q.c - c * _q.b + d * _q.a
			);
		}
		Quaternion operator*(double _a) const { return Quaternion(a * _a, b * _a, c * _a, d * _a); }
		friend Quaternion operator*(double _a, const Quaternion& _q) { return Quaternion(_a * _q.a, _a * _q.b, _a * _q.c, _a * _q.d); }

		bool operator==(const Quaternion& _q) const { return a == _q.a && b == _q.b && c == _q.c && d == _q.d; }
		bool operator==(double _a) const { return a == _a && b == 0 && c == 0 && d == 0; }
		bool operator!=(const Quaternion& _q) const { return !(*this == _q); }
		bool operator!=(double _a) const { return !(*this == _a); }

		/// @brief Returns conjugate quaternion.
		Quaternion Conjugate() const { return Quaternion(a, -b, -c, -d); }
		/// @brief Returns quaternion norm (magnitude).
		double Norm() const { return std::hypot(std::hypot(a, b), std::hypot(c, d)); }
		/// @brief Returns unit quaternion or zero quaternion if norm is near zero.
		Quaternion Unit() const {
			double factor = Norm();
			if (NumericConstants::IsNearZero(factor))
                return Quaternion(0, 0, 0, 0);
			factor = 1.0 / factor;
			return Quaternion(a * factor, b * factor, c * factor, d * factor);
		}
		/// @brief Returns multiplicative reciprocal, or zero quaternion if norm is near zero.
		Quaternion Reciprocal() const {
			double factor = Norm();
			if (NumericConstants::IsNearZero(factor))
                return Quaternion(0, 0, 0, 0);
			factor = 1.0 / (factor * factor);
			return Quaternion(a * factor, -b * factor, -c * factor, -d * factor);
		}

		/// @brief Builds a rotation quaternion from Euler angles (radians).
		/// @param rx Rotation around X axis.
		/// @param ry Rotation around Y axis.
		/// @param rz Rotation around Z axis.
		/// @return Rotation quaternion using project composition order.
		static Quaternion CreateRotation(double rx, double ry, double rz);

		/// @brief Builds rotation quaternion from Vector3 Euler angles.
		static Quaternion CreateRotation(const Vector3& angles) { return Quaternion::CreateRotation(angles.x, angles.y, angles.z); }
	};
}