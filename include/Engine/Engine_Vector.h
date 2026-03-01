#pragma once

#include "Engine_Geometry.h"
#include "Engine_Constant.h"

#include <math.h>

namespace Engine {
	/// @brief 3D vector type.
	struct Vector3 {
	public:
		/// @brief Vector components.
		double x = 0, y = 0, z = 0;

		Vector3() = default;
		Vector3(double _xyz) : x(_xyz), y(_xyz), z(_xyz) {}
		Vector3(double _x, double _y, double _z = 0.0) : x(_x), y(_y), z(_z) {}
		Vector3(const Point& _p) : x(_p.x), y(_p.y) {}
		Vector3(const Size& _s) : x(_s.w), y(_s.h) {}

		operator Point() const { return Point((int)x, (int)y); }
		operator Size() const { return Size((int)x, (int)y); }

		Vector3 operator-() const { return Vector3(-x, -y, -z); }
		
		Vector3& operator=(const Vector3& _v) { x = _v.x; y = _v.y; z = _v.z; return *this; }
		Vector3& operator=(double _xyz) { x = _xyz; y = _xyz; z = _xyz; return *this; }
		Vector3& operator+=(const Vector3& _v) { x += _v.x; y += _v.y; z += _v.z; return *this; }
		Vector3& operator+=(double _xyz) { x += _xyz; y += _xyz; z += _xyz; return *this; }
		Vector3& operator-=(const Vector3& _v) { x -= _v.x; y -= _v.y; z -= _v.z; return *this; }
		Vector3& operator-=(double _xyz) { x -= _xyz; y -= _xyz; z -= _xyz; return *this; }
		Vector3& operator*=(double _s) { x *= _s; y *= _s; z *= _s; return *this; }
		Vector3& operator*=(const Vector3& _v) {
			double _x = y * _v.z - z * _v.y;
			double _y = z * _v.x - x * _v.z;
			double _z = x * _v.y - y * _v.x;
			x = _x; y = _y; z = _z;
			return *this;
		}

		Vector3 operator+(const Vector3& _v) const { return Vector3(x + _v.x, y + _v.y, z + _v.z); }
		Vector3 operator+(double _xyz) const { return Vector3(x + _xyz, y + _xyz, z + _xyz); }
		friend Vector3 operator+(double _xyz, const Vector3& _v) { return Vector3(_xyz + _v.x, _xyz + _v.y, _xyz + _v.z); }
		Vector3 operator-(const Vector3& _v) const { return Vector3(x - _v.x, y - _v.y, z - _v.z); }
		Vector3 operator-(double _xyz) const { return Vector3(x - _xyz, y - _xyz, z - _xyz); }
		friend Vector3 operator-(double _xyz, const Vector3& _v) { return Vector3(_xyz - _v.x, _xyz - _v.y, _xyz - _v.z); }
		Vector3 operator*(double _s) const { return Vector3(x * _s, y * _s, z * _s); }
		friend Vector3 operator*(double _s, const Vector3& _v) { return Vector3(_s * _v.x, _s * _v.y, _s * _v.z); }
		Vector3 operator*(const Vector3& _v) const { return Vector3(y * _v.z - z * _v.y, z * _v.x - x * _v.z, x * _v.y - y * _v.x); }

		bool operator==(const Vector3& _v) const { return x == _v.x && y == _v.y && z == _v.z; }
		bool operator==(double _xyz) const { return x == _xyz && y == _xyz && z == _xyz; }
		bool operator!=(const Vector3& _v) const { return !(*this == _v); }
		bool operator!=(double _xyz) const { return !(*this == _xyz); }

		/// @brief Vector cross product with another vector.
		Vector3 CrossProductWith(const Vector3& r) const {
			return Vector3(y * r.z - z * r.y, z * r.x - x * r.z, x * r.y - y * r.x);
		}
		/// @brief Vector dot product with another vector.
		double DotProductWith(const Vector3& r) const {
			return x * r.x + y * r.y + z * r.z;
		}

		/// @brief Euclidean magnitude.
		double Magnitude() const { return std::hypot(x, std::hypot(y, z)); }
		/// @brief Returns normalized vector or zero vector when magnitude is near zero.
		Vector3 Normalize() const {
			// Why normal like this? Because CPU do sqrt very fast nowaday so no need fast-inverse square root.
			double factor = Magnitude();
			if (NumericConstants::IsNearZero(factor))
				return Vector3(0, 0, 0); // Return zero Vector 3 as failure, the 'magnitude' is near 0.
			factor = 1.0 / factor;
			return Vector3(x * factor, y * factor, z * factor);
		}

		/// @brief Static cross product helper.
		static Vector3 CrossProduct(const Vector3& l, const Vector3& r) {
			return Vector3(l.y * r.z - l.z * r.y, l.z * r.x - l.x * r.z, l.x * r.y - l.y * r.x);
		}
		/// @brief Static dot product helper.
		static double DotProduct(const Vector3& l, const Vector3& r) {
			return l.x * r.x + l.y * r.y + l.z * r.z;
		}
	};

	/// @brief 2D vector type.
	struct Vector2 {
	public:
		/// @brief Vector components.
		double x = 0, y = 0;
		
		Vector2() = default;
		Vector2(double _xy) : x(_xy), y(_xy) {}
		Vector2(double _x, double _y) : x(_x), y(_y) {}
		Vector2(const Vector3& _v) : x(_v.x), y(_v.y) {}
		Vector2(const Point& _p) : x(_p.x), y(_p.y) {}
		Vector2(const Size& _s) : x(_s.w), y(_s.h) {}

		operator Point() const { return Point((int)x, (int)y); }
		operator Size() const { return Size((int)x, (int)y); }
		operator Vector3() const { return Vector3(x, y, 0); }

		Vector2& operator=(const Vector2& _v) { x = _v.x; y = _v.y; return *this; }
		Vector2& operator=(double _xy) { x = _xy; y = _xy; return *this; }
		Vector2& operator+=(const Vector2& _v) { x += _v.x; y += _v.y; return *this; }
		Vector2& operator+=(double _xy) { x += _xy; y += _xy; return *this; }
		Vector2& operator-=(const Vector2& _v) { x -= _v.x; y -= _v.y; return *this; }
		Vector2& operator-=(double _xy) { x -= _xy; y -= _xy; return *this; }
		Vector2& operator*=(double _s) { x *= _s; y *= _s; return *this; }

		Vector2 operator+(const Vector2& _v) const { return Vector2(x + _v.x, y + _v.y); }
		Vector2 operator+(double _xy) const { return Vector2(x + _xy, y + _xy); }
		friend Vector2 operator+(double _xy, const Vector2& _v) { return Vector2(_xy + _v.x, _xy + _v.y); }
		Vector2 operator-(const Vector2& _v) const { return Vector2(x - _v.x, y - _v.y); }
		Vector2 operator-(double _xy) const { return Vector2(x - _xy, y - _xy); }
		friend Vector2 operator-(double _xy, const Vector2& _v) { return Vector2(_xy - _v.x, _xy - _v.y); }
		Vector2 operator*(double _s) const { return Vector2(x * _s, y * _s); }
		friend Vector2 operator*(double _s, const Vector2& _v) { return Vector2(_s * _v.x, _s * _v.y); }
		Vector3 operator*(const Vector2& _v) const { return Vector3(0, 0, x * _v.y - y * _v.x); }
		Vector3 operator*(const Vector3& _r) const { return Vector3(y * _r.z, -x * _r.z, x * _r.y - y * _r.x); }
		friend Vector3 operator*(const Vector3& l, const Vector2& r) { return Vector3(-l.z * r.y, l.z * r.x, l.x * r.y - l.y * r.x); }

		bool operator==(const Vector2& _v) const { return x == _v.x && y == _v.y; }
		bool operator==(double _xy) const { return x == _xy && y == _xy; }
		bool operator!=(const Vector2& _v) const { return !(*this == _v); }
		bool operator!=(double _xy) const { return !(*this == _xy); }

		/// @brief Cross product with Vector3 (returns Vector3).
		Vector3 CrossProductWith(const Vector3& r) const { return Vector3(y * r.z, -x * r.z, x * r.y - y * r.x); }
		/// @brief Cross product with Vector2 (returns z-axis Vector3).
		Vector3 CrossProductWith(const Vector2& r) const { return Vector3(0, 0, x * r.y - y * r.x); }
		/// @brief Dot product with Vector3 (ignoring z).
		double DotProductWith(const Vector3& r) const { return x * r.x + y * r.y; }
		/// @brief Dot product with Vector2.
		double DotProductWith(const Vector2& r) const { return x * r.x + y * r.y; }

		/// @brief Euclidean magnitude.
		double Magnitude() const { return std::hypot(x, y); }
		/// @brief Returns normalized vector or zero vector when magnitude is near zero.
		Vector2 Normalize() const {
			double factor = Magnitude();
			if (NumericConstants::IsNearZero(factor))
				return Vector2(0, 0); // Return zero Vector 2 as failure, the 'magnitude' is near 0.
			factor = 1.0 / factor;
			return Vector2(x * factor, y * factor);
		}

		/// @brief Static cross product helper.
		static Vector3 CrossProduct(const Vector2& l, const Vector2& r) { return Vector3(0, 0, l.x * r.y - l.y * r.x); }
		/// @brief Static dot product helper.
		static double DotProduct(const Vector2& l, const Vector2& r) { return l.x * r.x + l.y * r.y; }
	};

	/// @brief 4D vector type.
	struct Vector4 {
	public:
		/// @brief Vector components.
		double x = 0, y = 0, z = 0, w = 0;

		Vector4() = default;
		Vector4(double _xyzw) : x(_xyzw), y(_xyzw), z(_xyzw), w(_xyzw) {}
		Vector4(double _x, double _y, double _z = 0.0, double _w = 0.0) : x(_x), y(_y), z(_z), w(_w) {}
		Vector4(const Vector2& _v) : x(_v.x), y(_v.y) {}
		Vector4(const Vector3& _v) : x(_v.x), y(_v.y), z(_v.z) {}
		Vector4(const Vector2& _v, double _z, double _w = 0.0) : x(_v.x), y(_v.y), z(_z), w(_w) {}
		Vector4(const Vector3& _v, double _w) : x(_v.x), y(_v.y), z(_v.z), w(_w) {}

		operator Vector2() const { return Vector2(x, y); }
		operator Vector3() const { return Vector3(x, y, z); }

		Vector4 operator-() const { return Vector4(-x, -y, -z, -w); }
		
		Vector4& operator=(const Vector4& _v) { x = _v.x; y = _v.y; z = _v.z; w = _v.w; return *this; }
		Vector4& operator=(double _xyzw) { x = _xyzw; y = _xyzw; z = _xyzw; w = _xyzw; return *this; }
		Vector4& operator+=(const Vector4& _v) { x += _v.x; y += _v.y; z += _v.z; w += _v.w; return *this; }
		Vector4& operator+=(double _xyzw) { x += _xyzw; y += _xyzw; z += _xyzw; w += _xyzw; return *this; }
		Vector4& operator-=(const Vector4& _v) { x -= _v.x; y -= _v.y; z -= _v.z; w -= _v.w; return *this; }
		Vector4& operator-=(double _xyzw) { x -= _xyzw; y -= _xyzw; z -= _xyzw; w -= _xyzw; return *this; }
		Vector4& operator*=(double _s) { x *= _s; y *= _s; z *= _s; w *= _s; return *this; }

		Vector4 operator+(const Vector4& _v) const { return Vector4(x + _v.x, y + _v.y, z + _v.z, w + _v.w); }
		Vector4 operator+(double _xyzw) const { return Vector4(x + _xyzw, y + _xyzw, z + _xyzw, w + _xyzw); }
		friend Vector4 operator+(double _xyzw, const Vector4& _v) { return Vector4(_xyzw + _v.x, _xyzw + _v.y, _xyzw + _v.z, _xyzw + _v.w); }
		Vector4 operator-(const Vector4& _v) const { return Vector4(x - _v.x, y - _v.y, z - _v.z, w - _v.w); }
		Vector4 operator-(double _xyzw) const { return Vector4(x - _xyzw, y - _xyzw, z - _xyzw, w - _xyzw); }
		friend Vector4 operator-(double _xyzw, const Vector4& _v) { return Vector4(_xyzw - _v.x, _xyzw - _v.y, _xyzw - _v.z, _xyzw - _v.w); }
		Vector4 operator*(double _s) const { return Vector4(x * _s, y * _s, z * _s, w * _s); }
		friend Vector4 operator*(double _s, const Vector4& _v) { return Vector4(_s * _v.x, _s * _v.y, _s * _v.z, _s * _v.w); }

		bool operator==(const Vector4& _v) const { return x == _v.x && y == _v.y && z == _v.z && w == _v.w; }
		bool operator==(double _xyzw) const { return x == _xyzw && y == _xyzw && z == _xyzw && w == _xyzw; }
		bool operator!=(const Vector4& _v) const { return !(*this == _v); }
		bool operator!=(double _xyzw) const { return !(*this == _xyzw); }

		/// @brief Dot product with another Vector4.
		double DotProductWith(const Vector4& _v) const {
			return x * _v.x + y * _v.y + z * _v.z + w * _v.w;
		}
		/// @brief Euclidean magnitude.
		double Magnitude() const { return std::hypot(std::hypot(x, y), std::hypot(z, w)); }
		/// @brief Returns normalized vector or zero vector when magnitude is near zero.
		Vector4 Normalize() const {
			double factor = Magnitude();
			if (NumericConstants::IsNearZero(factor))
				return Vector4(0, 0, 0, 0);
			factor = 1.0 / factor;
			return Vector4(x * factor, y * factor, z * factor, w * factor);
		}

		/// @brief Static dot product helper.
		static double DotProduct(const Vector4& l, const Vector4& r) {
			return l.x * r.x + l.y * r.y + l.z * r.z + l.w * r.w;
		}

	};
}