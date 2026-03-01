#pragma once

#include "Engine_Quaternion.h"
#include "Engine_Vector.h"
#include "Engine_Constant.h"

#include <cmath>
#include <math.h>
#include <array>
#include <functional>

namespace Engine {
	/// @brief 4x4 matrix with row-major storage.
	struct Matrix4x4 {
	private:
		std::array<double, 16> data;
	public:
		Matrix4x4() { data.fill(0); }
		Matrix4x4(double _fill) { data.fill(_fill); }
		Matrix4x4(const std::array<double, 16>& _data) : data(_data) {}

		Matrix4x4 operator-() const {
			std::array<double, 16> _tmp;
			for (int i = 0; i < 16; ++i) _tmp[i] = -data[i];
			return Matrix4x4(_tmp);
		}
		double& operator()(int r, int c) { return data[r * 4 + c]; }
		const double& operator()(int r, int c) const { return data[r * 4 + c]; }
		
		Matrix4x4& operator=(const Matrix4x4& _m) { data = _m.data; return *this; }
		Matrix4x4& operator=(const std::array<double, 16>& _m) { data = _m; return *this; }
		Matrix4x4& operator=(double _fill) { data.fill(_fill); return *this; }
		Matrix4x4& operator+=(const std::array<double, 16>& _m) { Matrix4x4::Add(data, _m, data); return *this; }
		Matrix4x4& operator+=(const Matrix4x4& _m) { Matrix4x4::Add(data, _m.data, data); return *this; }
		Matrix4x4& operator+=(double _v) {
			for (double& v : data) v += _v;
			return *this;
		}
		Matrix4x4& operator-=(const std::array<double, 16>& _m) { Matrix4x4::Subtract(data, _m, data); return *this; }
		Matrix4x4& operator-=(const Matrix4x4& _m) { Matrix4x4::Subtract(data, _m.data, data); return *this; }
		Matrix4x4& operator-=(double _v) {
			for (double& v : data) v -= _v;
			return *this;
		}

		Matrix4x4& operator*=(double _s) {
			for (double& v : data) v *= _s;
			return *this;
		}
		Matrix4x4& operator*=(const std::array<double, 16>& _m) {
			std::array<double, 16> _tmp;
			Matrix4x4::Multiply(data, _m, _tmp);
			data.swap(_tmp);
			return *this;
		}
		Matrix4x4& operator*=(const Matrix4x4& _m) {
			std::array<double, 16> _tmp;
			Matrix4x4::Multiply(data, _m.data, _tmp);
			data.swap(_tmp);
			return *this;
		}

		Matrix4x4 operator+(const Matrix4x4& _m) const {
			std::array<double, 16> _tmp;
			Matrix4x4::Add(data, _m.data, _tmp);
			return Matrix4x4(_tmp);
		}
		Matrix4x4 operator+(const std::array<double, 16>& _m) const {
			std::array<double, 16> _tmp;
			Matrix4x4::Add(data, _m, _tmp);
			return Matrix4x4(_tmp);
		}
		Matrix4x4 operator+(double _v) const {
			std::array<double, 16> _tmp;
			for (int i = 0; i < 16; ++i) _tmp[i] = data[i] + _v;
			return Matrix4x4(_tmp);
		}
		friend Matrix4x4 operator+(const std::array<double, 16>& l, const Matrix4x4& r) {
			std::array<double, 16> _tmp;
			Matrix4x4::Add(l, r.data, _tmp);
			return Matrix4x4(_tmp);
		}
		friend Matrix4x4 operator+(double _v, const Matrix4x4& r) {
			std::array<double, 16> _tmp;
			for (int i = 0; i < 16; ++i) _tmp[i] = _v + r.data[i];
			return Matrix4x4(_tmp);
		}

		Matrix4x4 operator-(const Matrix4x4& _m) const {
			std::array<double, 16> _tmp;
			Matrix4x4::Subtract(data, _m.data, _tmp);
			return Matrix4x4(_tmp);
		}
		Matrix4x4 operator-(const std::array<double, 16>& _m) const {
			std::array<double, 16> _tmp;
			Matrix4x4::Subtract(data, _m, _tmp);
			return Matrix4x4(_tmp);
		}
		Matrix4x4 operator-(double _v) const {
			std::array<double, 16> _tmp;
			for (int i = 0; i < 16; ++i) _tmp[i] = data[i] - _v;
			return Matrix4x4(_tmp);
		}
		friend Matrix4x4 operator-(const std::array<double, 16>& l, const Matrix4x4& r) {
			std::array<double, 16> _tmp;
			Matrix4x4::Subtract(l, r.data, _tmp);
			return Matrix4x4(_tmp);
		}
		friend Matrix4x4 operator-(double _v, const Matrix4x4& r) {
			std::array<double, 16> _tmp;
			for (int i = 0; i < 16; ++i) _tmp[i] = _v - r.data[i];
			return Matrix4x4(_tmp);
		}

		Matrix4x4 operator*(const Matrix4x4& _m) const {
			std::array<double, 16> _tmp;
			Matrix4x4::Multiply(data, _m.data, _tmp);
			return Matrix4x4(_tmp);
		}
		Matrix4x4 operator*(const std::array<double, 16>& _m) const {
			std::array<double, 16> _tmp;
			Matrix4x4::Multiply(data, _m, _tmp);
			return Matrix4x4(_tmp);
		}
		Matrix4x4 operator*(double _v) const {
			std::array<double, 16> _tmp;
			for (int i = 0; i < 16; ++i) _tmp[i] = data[i] * _v;
			return Matrix4x4(_tmp);
		}
		friend Matrix4x4 operator*(const std::array<double, 16>& l, const Matrix4x4& r) {
			std::array<double, 16> _tmp;
			Matrix4x4::Multiply(l, r.data, _tmp);
			return Matrix4x4(_tmp);
		}
		friend Matrix4x4 operator*(double _v, const Matrix4x4& r) {
			std::array<double, 16> _tmp;
			for (int i = 0; i < 16; ++i) _tmp[i] = _v * r.data[i];
			return Matrix4x4(_tmp);
		}

		Vector4 operator*(const Vector4& _v) const {
			return Vector4(
				data[0] * _v.x + data[1] * _v.y + data[2] * _v.z + data[3] * _v.w,
				data[4] * _v.x + data[5] * _v.y + data[6] * _v.z + data[7] * _v.w,
				data[8] * _v.x + data[9] * _v.y + data[10] * _v.z + data[11] * _v.w,
				data[12] * _v.x + data[13] * _v.y + data[14] * _v.z + data[15] * _v.w
			);
		}

		double& operator[](int i) { return data[i]; }
		const double& operator[](int i) const { return data[i]; }

		std::array<double, 16>& Data() { return data; }
		const std::array<double, 16>& Data() const { return data; }

		static void Add(const std::array<double, 16>& l, const std::array<double, 16>& r, std::array<double, 16>& res) {
			std::transform(l.cbegin(), l.cend(), r.cbegin(), res.begin(), std::plus<double>());
		}
		static void Add(const Matrix4x4& l, const Matrix4x4& r, Matrix4x4& res) { Matrix4x4::Add(l.data, r.data, res.data); }

		static void Subtract(const std::array<double, 16>& l, const std::array<double, 16>& r, std::array<double, 16>& res) {
			std::transform(l.cbegin(), l.cend(), r.cbegin(), res.begin(), std::minus<double>());
		}
		static void Subtract(const Matrix4x4& l, const Matrix4x4& r, Matrix4x4& res) { Matrix4x4::Subtract(l.data, r.data, res.data); }

		static void Multiply(const std::array<double, 16>& l, const std::array<double, 16>& r, std::array<double, 16>& res) {
			auto res_ptr = res.begin();
			for (int _r = 0; _r < 4; ++_r) {
				for (int _c = 0; _c < 4; ++_c, ++res_ptr)
					(*res_ptr) = (l[_r * 4] * r[_c]) + (l[_r * 4 + 1] * r[_c + 4]) + (l[_r * 4 + 2] * r[_c + 8]) + (l[_r * 4 + 3] * r[_c + 12]);
			}
		}
		static void Multiply(const Matrix4x4& l, const Matrix4x4& r, Matrix4x4& res) { Matrix4x4::Multiply(l.data, r.data, res.data); }

		static void GetIdentity(std::array<double, 16>& res, double scalar = 1.0) {
			std::array<double, 16> _tmp = {scalar, 0, 0, 0,
						       0, scalar, 0, 0,
						       0, 0, scalar, 0,
						       0, 0, 0, scalar};
			res.swap(_tmp);
		}
		static void GetIdentity(Matrix4x4& res, double scalar = 1.0) { Matrix4x4::GetIdentity(res.data, scalar); }
		static Matrix4x4 GetIdentity(double scalar = 1.0) {
			std::array<double, 16> _tmp;
			Matrix4x4::GetIdentity(_tmp, scalar);
			return Matrix4x4(_tmp);
		}

		static void GetTranslation3D(std::array<double, 16>& res, const Vector3& amount) {
			std::array<double, 16> _tmp = {1, 0, 0, amount.x,
						       0, 1, 0, amount.y,
						       0, 0, 1, amount.z,
						       0, 0, 0, 1};
			res.swap(_tmp);
		}
		static void GetTranslation3D(Matrix4x4& res, const Vector3& amount) { Matrix4x4::GetTranslation3D(res.data, amount); }
		static Matrix4x4 GetTranslation3D(const Vector3& amount) {
			std::array<double, 16> _tmp;
			Matrix4x4::GetTranslation3D(_tmp, amount);
			return Matrix4x4(_tmp);
		}

		static void GetScale3D(std::array<double, 16>& res, const Vector3& amount) {
			std::array<double, 16> _tmp = {amount.x, 0, 0, 0,
						       0, amount.y, 0, 0,
						       0, 0, amount.z, 0,
						       0, 0, 0, 1};
			res.swap(_tmp);
		}
		static void GetScale3D(Matrix4x4& res, const Vector3& amount) { Matrix4x4::GetScale3D(res.data, amount); }
		static Matrix4x4 GetScale3D(const Vector3& amount) {
			std::array<double, 16> _tmp;
			Matrix4x4::GetScale3D(_tmp, amount);
			return Matrix4x4(_tmp);
		}

		static void GetRotation3D(std::array<double, 16>& res, const Quaternion& rotation) {
			Quaternion q = rotation.Unit(); // On failure, this will return a zero quaternion, which in will result in return the identity matrix.

			double bb = q.b * q.b, cc = q.c * q.c, dd = q.d * q.d;
			double bc = q.b * q.c, bd = q.b * q.d, cd = q.c * q.d;
			double ad = q.a * q.d, ac = q.a * q.c, ab = q.a * q.b;

			std::array<double, 16> _tmp = {1 - 2 * (cc + dd), 2 * (bc - ad), 2 * (bd + ac), 0,
						       2 * (bc + ad), 1 - 2 * (bb + dd), 2 * (cd - ab), 0,
						       2 * (bd - ac), 2 * (cd + ab), 1 - 2 * (bb + cc), 0,
						       0, 0, 0, 1};
			res.swap(_tmp);
		}
		static void GetRotation3D(std::array<double, 16>& res, const Vector3& angles) { Matrix4x4::GetRotation3D(res, Quaternion::CreateRotation(angles)); }
		static void GetRotation3D(std::array<double, 16>& res, double angle_x, double angle_y, double angle_z) { Matrix4x4::GetRotation3D(res, Quaternion::CreateRotation(angle_x, angle_y, angle_z)); }
		static void GetRotation3D(Matrix4x4& res, const Quaternion& rotation) { Matrix4x4::GetRotation3D(res.data, rotation); }
		static void GetRotation3D(Matrix4x4& res, const Vector3& angles) { Matrix4x4::GetRotation3D(res.data, angles); }
		static void GetRotation3D(Matrix4x4& res, double angle_x, double angle_y, double angle_z) { Matrix4x4::GetRotation3D(res.data, angle_x, angle_y, angle_z); }
		static Matrix4x4 GetRotation3D(const Quaternion& rotation) {
			std::array<double, 16> _tmp;
			Matrix4x4::GetRotation3D(_tmp, rotation);
			return Matrix4x4(_tmp);
		}
		static Matrix4x4 GetRotation3D(const Vector3& angles) {
			std::array<double, 16> _tmp;
			Matrix4x4::GetRotation3D(_tmp, angles);
			return Matrix4x4(_tmp);
		}
		static Matrix4x4 GetRotation3D(double angle_x, double angle_y, double angle_z) {
			std::array<double, 16> _tmp;
			Matrix4x4::GetRotation3D(_tmp, angle_x, angle_y, angle_z);
			return Matrix4x4(_tmp);
		}

		/// @brief Builds combined 3D transform matrix (scale, rotation, translation).
		static void GetTransformation3D(std::array<double, 16>& res, const Vector3& translation, const Vector3& rotation, const Vector3& scale);
		static void GetTransformation3D(Matrix4x4& res, const Vector3& translation, const Vector3& rotation, const Vector3& scale) {
			Matrix4x4::GetTransformation3D(res.data, translation, rotation, scale);
		}
		static Matrix4x4 GetTransformation3D(const Vector3& translation, const Vector3& rotation, const Vector3& scale) {
			std::array<double, 16> _tmp;
			Matrix4x4::GetTransformation3D(_tmp, translation, rotation, scale);
			return Matrix4x4(_tmp);
		}

		/// @brief Builds frustum projection matrix.
		static void GetFrustumProjection(std::array<double, 16>& res, double left, double right, double top, double bottom, double near, double far);
		static void GetFrustumProjection(Matrix4x4& res, double left, double right, double top, double bottom, double near, double far) {
			Matrix4x4::GetFrustumProjection(res.data, left, right, top, bottom, near, far);
		}
		static Matrix4x4 GetFrustumProjection(double left, double right, double top, double bottom, double near, double far) {
			std::array<double, 16> _tmp;
			Matrix4x4::GetFrustumProjection(_tmp, left, right, top, bottom, near, far);
			return Matrix4x4(_tmp);
		}
		
		/// @brief Builds perspective projection matrix.
		static void GetPerspectiveProjection(std::array<double, 16>& res, double fovY, double aspect, double near, double far);
		static void GetPerspectiveProjection(Matrix4x4& res, double fovY, double aspect, double near, double far) {
			Matrix4x4::GetPerspectiveProjection(res.data, fovY, aspect, near, far);
		}
		static Matrix4x4 GetPerspectiveProjection(double fovY, double aspect, double near, double far) {
			std::array<double, 16> _tmp;
			Matrix4x4::GetPerspectiveProjection(_tmp, fovY, aspect, near, far);
			return Matrix4x4(_tmp);
		}

		/// @brief Builds orthographic projection matrix.
		static void GetOrthographicProjection(std::array<double, 16>& res, double left, double right, double top, double bottom, double near, double far);
		static void GetOrthographicProjection(Matrix4x4& res, double left, double right, double top, double bottom, double near, double far) {
			Matrix4x4::GetOrthographicProjection(res.data, left, right, top, bottom, near, far);
		}
		static Matrix4x4 GetOrthographicProjection(double left, double right, double top, double bottom, double near, double far) {
			std::array<double, 16> _tmp;
			Matrix4x4::GetOrthographicProjection(_tmp, left, right, top, bottom, near, far);
			return Matrix4x4(_tmp);
		}
	};

	/// @brief 3x3 matrix with row-major storage.
	struct Matrix3x3 {
	private:
		std::array<double, 9> data;
	public:
		Matrix3x3() { data.fill(0); }
		Matrix3x3(double _fill) { data.fill(_fill); }
		Matrix3x3(const std::array<double, 9>& _data) : data(_data) {}

		Matrix3x3 operator-() const {
			std::array<double, 9> _tmp;
			for (int i = 0; i < 9; ++i) _tmp[i] = -data[i];
			return Matrix3x3(_tmp);
		}
		double& operator()(int r, int c) { return data[r * 3 + c]; }
		const double& operator()(int r, int c) const { return data[r * 3 + c]; }
		
		Matrix3x3& operator=(const Matrix3x3& _m) { data = _m.data; return *this; }
		Matrix3x3& operator=(const std::array<double, 9>& _m) { data = _m; return *this; }
		Matrix3x3& operator=(double _fill) { data.fill(_fill); return *this; }
		Matrix3x3& operator+=(const std::array<double, 9>& _m) { Matrix3x3::Add(data, _m, data); return *this; }
		Matrix3x3& operator+=(const Matrix3x3& _m) { Matrix3x3::Add(data, _m.data, data); return *this; }
		Matrix3x3& operator+=(double _v) {
			for (double& v : data) v += _v;
			return *this;
		}
		Matrix3x3& operator-=(const std::array<double, 9>& _m) { Matrix3x3::Subtract(data, _m, data); return *this; }
		Matrix3x3& operator-=(const Matrix3x3& _m) { Matrix3x3::Subtract(data, _m.data, data); return *this; }
		Matrix3x3& operator-=(double _v) {
			for (double& v : data) v -= _v;
			return *this;
		}

		Matrix3x3& operator*=(double _s) {
			for (double& v : data) v *= _s;
			return *this;
		}
		Matrix3x3& operator*=(const std::array<double, 9>& _m) {
			std::array<double, 9> _tmp;
			Matrix3x3::Multiply(data, _m, _tmp);
			data.swap(_tmp);
			return *this;
		}
		Matrix3x3& operator*=(const Matrix3x3& _m) {
			std::array<double, 9> _tmp;
			Matrix3x3::Multiply(data, _m.data, _tmp);
			data.swap(_tmp);
			return *this;
		}

		Matrix3x3 operator+(const Matrix3x3& _m) const {
			std::array<double, 9> _tmp;
			Matrix3x3::Add(data, _m.data, _tmp);
			return Matrix3x3(_tmp);
		}
		Matrix3x3 operator+(const std::array<double, 9>& _m) const {
			std::array<double, 9> _tmp;
			Matrix3x3::Add(data, _m, _tmp);
			return Matrix3x3(_tmp);
		}
		Matrix3x3 operator+(double _v) const {
			std::array<double, 9> _tmp;
			for (int i = 0; i < 9; ++i) _tmp[i] = data[i] + _v;
			return Matrix3x3(_tmp);
		}
		friend Matrix3x3 operator+(const std::array<double, 9>& l, const Matrix3x3& r) {
			std::array<double, 9> _tmp;
			Matrix3x3::Add(l, r.data, _tmp);
			return Matrix3x3(_tmp);
		}
		friend Matrix3x3 operator+(double _v, const Matrix3x3& r) {
			std::array<double, 9> _tmp;
			for (int i = 0; i < 9; ++i) _tmp[i] = _v + r.data[i];
			return Matrix3x3(_tmp);
		}

		Matrix3x3 operator-(const Matrix3x3& _m) const {
			std::array<double, 9> _tmp;
			Matrix3x3::Subtract(data, _m.data, _tmp);
			return Matrix3x3(_tmp);
		}
		Matrix3x3 operator-(const std::array<double, 9>& _m) const {
			std::array<double, 9> _tmp;
			Matrix3x3::Subtract(data, _m, _tmp);
			return Matrix3x3(_tmp);
		}
		Matrix3x3 operator-(double _v) const {
			std::array<double, 9> _tmp;
			for (int i = 0; i < 9; ++i) _tmp[i] = data[i] - _v;
			return Matrix3x3(_tmp);
		}
		friend Matrix3x3 operator-(const std::array<double, 9>& l, const Matrix3x3& r) {
			std::array<double, 9> _tmp;
			Matrix3x3::Subtract(l, r.data, _tmp);
			return Matrix3x3(_tmp);
		}
		friend Matrix3x3 operator-(double _v, const Matrix3x3& r) {
			std::array<double, 9> _tmp;
			for (int i = 0; i < 9; ++i) _tmp[i] = _v - r.data[i];
			return Matrix3x3(_tmp);
		}

		Matrix3x3 operator*(const Matrix3x3& _m) const {
			std::array<double, 9> _tmp;
			Matrix3x3::Multiply(data, _m.data, _tmp);
			return Matrix3x3(_tmp);
		}
		Matrix3x3 operator*(const std::array<double, 9>& _m) const {
			std::array<double, 9> _tmp;
			Matrix3x3::Multiply(data, _m, _tmp);
			return Matrix3x3(_tmp);
		}
		Matrix3x3 operator*(double _v) const {
			std::array<double, 9> _tmp;
			for (int i = 0; i < 9; ++i) _tmp[i] = data[i] * _v;
			return Matrix3x3(_tmp);
		}
		friend Matrix3x3 operator*(const std::array<double, 9>& l, const Matrix3x3& r) {
			std::array<double, 9> _tmp;
			Matrix3x3::Multiply(l, r.data, _tmp);
			return Matrix3x3(_tmp);
		}
		friend Matrix3x3 operator*(double _v, const Matrix3x3& r) {
			std::array<double, 9> _tmp;
			for (int i = 0; i < 9; ++i) _tmp[i] = _v * r.data[i];
			return Matrix3x3(_tmp);
		}

		Vector3 operator*(const Vector3& _v) const {
			return Vector3(
				data[0] * _v.x + data[1] * _v.y + data[2] * _v.z,
				data[3] * _v.x + data[4] * _v.y + data[5] * _v.z,
				data[6] * _v.x + data[7] * _v.y + data[8] * _v.z
			);
		}

		double& operator[](int i) { return data[i]; }
		const double& operator[](int i) const { return data[i]; }

		std::array<double, 9>& Data() { return data; }
		const std::array<double, 9>& Data() const { return data; }

		static void Add(const std::array<double, 9>& l, const std::array<double, 9>& r, std::array<double, 9>& res) {
			std::transform(l.cbegin(), l.cend(), r.cbegin(), res.begin(), std::plus<double>());
		}
		static void Add(const Matrix3x3& l, const Matrix3x3& r, Matrix3x3& res) { Matrix3x3::Add(l.data, r.data, res.data); }

		static void Subtract(const std::array<double, 9>& l, const std::array<double, 9>& r, std::array<double, 9>& res) {
			std::transform(l.cbegin(), l.cend(), r.cbegin(), res.begin(), std::minus<double>());
		}
		static void Subtract(const Matrix3x3& l, const Matrix3x3& r, Matrix3x3& res) { Matrix3x3::Subtract(l.data, r.data, res.data); }

		static void Multiply(const std::array<double, 9>& l, const std::array<double, 9>& r, std::array<double, 9>& res) {
			auto res_ptr = res.begin();
			for (int _r = 0; _r < 3; ++_r) {
				for (int _c = 0; _c < 3; ++_c, ++res_ptr)
					(*res_ptr) = (l[_r * 3] * r[_c]) + (l[_r * 3 + 1] * r[_c + 3]) + (l[_r * 3 + 2] * r[_c + 6]);
			}
		}
		static void Multiply(const Matrix3x3& l, const Matrix3x3& r, Matrix3x3& res) { Matrix3x3::Multiply(l.data, r.data, res.data); }

		static void GetIdentity(std::array<double, 9>& res, double scalar = 1.0) {
			std::array<double, 9> _tmp = {scalar, 0, 0,
										  0, scalar, 0,
										  0, 0, scalar};
			res.swap(_tmp);
		}
		static void GetIdentity(Matrix3x3& res, double scalar = 1.0) { Matrix3x3::GetIdentity(res.data, scalar); }
		static Matrix3x3 GetIdentity(double scalar = 1.0) {
			std::array<double, 9> _tmp;
			Matrix3x3::GetIdentity(_tmp, scalar);
			return Matrix3x3(_tmp);
		}

		static void GetTranslation2D(std::array<double, 9>& res, const Vector2& amount) {
			std::array<double, 9> _tmp = {1, 0, amount.x,
						       			  0, 1, amount.y,
						       			  0, 0, 1};
			res.swap(_tmp);
		}
		static void GetTranslation2D(Matrix3x3& res, const Vector3& amount) { Matrix3x3::GetTranslation2D(res.data, amount); }
		static Matrix3x3 GetTranslation2D(const Vector3& amount) {
			std::array<double, 9> _tmp;
			Matrix3x3::GetTranslation2D(_tmp, amount);
			return Matrix3x3(_tmp);
		}

		static void GetScale2D(std::array<double, 9>& res, const Vector2& amount) {
			std::array<double, 9> _tmp = {amount.x, 0, 0,
						       			  0, amount.y, 0,
						       			  0, 0, 1};
			res.swap(_tmp);
		}
		static void GetScale2D(Matrix3x3& res, const Vector2& amount) { Matrix3x3::GetScale2D(res.data, amount); }
		static Matrix3x3 GetScale2D(const Vector2& amount) {
			std::array<double, 9> _tmp;
			Matrix3x3::GetScale2D(_tmp, amount);
			return Matrix3x3(_tmp);
		}

		static void GetRotation2D(std::array<double, 9>& res, double angle) {
			double s = std::sin(angle), c = std::cos(angle);
			std::array<double, 9> _tmp = {c, -s, 0,
										  s, c, 0,
										  0, 0, 1};
			res.swap(_tmp);
		}
		static void GetRotation2D(Matrix3x3& res, double angle) { Matrix3x3::GetRotation2D(res.data, angle); }
		static Matrix3x3 GetRotation2D(double angle) {
			std::array<double, 9> _tmp;
			Matrix3x3::GetRotation2D(_tmp, angle);
			return Matrix3x3(_tmp);
		}

		/// @brief Builds combined 2D homogeneous transform matrix.
		static void GetTransformation2D(std::array<double, 9>& res, const Vector3& translation, double rotation_angle, const Vector3& scale);
		static void GetTransformation2D(Matrix3x3& res, const Vector3& translation, double rotation_angle, const Vector3& scale) {
			Matrix3x3::GetTransformation2D(res.data, translation, rotation_angle, scale);
		}
		static Matrix3x3 GetTransformation2D(const Vector3& translation, double rotation_angle, const Vector3& scale) {
			std::array<double, 9> _tmp;
			Matrix3x3::GetTransformation2D(_tmp, translation, rotation_angle, scale);
			return Matrix3x3(_tmp);
		}

		/// @brief Builds 2D viewport transform matrix.
		static void GetViewportTransform2D(std::array<double, 9>& res, double left_i, double right_i, double top_i, double bottom_i, double left_o, double right_o, double top_o, double bottom_o);
		static void GetViewportTransform2D(Matrix3x3& res, double left_i, double right_i, double top_i, double bottom_i, double left_o, double right_o, double top_o, double bottom_o) {
			Matrix3x3::GetViewportTransform2D(res.data, left_i, right_i, top_i, bottom_i, left_o, right_o, top_o, bottom_o);
		}
		static Matrix3x3 GetViewportTransform2D(double left_i, double right_i, double top_i, double bottom_i, double left_o, double right_o, double top_o, double bottom_o) {
			std::array<double, 9> _tmp;
			Matrix3x3::GetViewportTransform2D(_tmp, left_i, right_i, top_i, bottom_i, left_o, right_o, top_o, bottom_o);
			return Matrix3x3(_tmp);
		}

		static void GetScale3D(std::array<double, 9>& res, const Vector3& amount) {
			std::array<double, 9> _tmp = {amount.x, 0, 0,
						       0, amount.y, 0,
						       0, 0, amount.z};
			res.swap(_tmp);
		}
		static void GetScale3D(Matrix3x3& res, const Vector3& amount) { Matrix3x3::GetScale3D(res.data, amount); }
		static Matrix3x3 GetScale3D(const Vector3& amount) {
			std::array<double, 9> _tmp;
			Matrix3x3::GetScale3D(_tmp, amount);
			return Matrix3x3(_tmp);
		}

		static void GetRotation3D(std::array<double, 9>& res, const Quaternion& rotation) {
			Quaternion q = rotation.Unit(); // On failure, this will return a zero quaternion, which in will result in return the identity matrix.

			double bb = q.b * q.b, cc = q.c * q.c, dd = q.d * q.d;
			double bc = q.b * q.c, bd = q.b * q.d, cd = q.c * q.d;
			double ad = q.a * q.d, ac = q.a * q.c, ab = q.a * q.b;

			std::array<double, 9> _tmp = {1 - 2 * (cc + dd), 2 * (bc - ad), 2 * (bd + ac),
						       2 * (bc + ad), 1 - 2 * (bb + dd), 2 * (cd - ab),
						       2 * (bd - ac), 2 * (cd + ab), 1 - 2 * (bb + cc)};
			res.swap(_tmp);
		}
		static void GetRotation3D(std::array<double, 9>& res, const Vector3& angles) { Matrix3x3::GetRotation3D(res, Quaternion::CreateRotation(angles)); }
		static void GetRotation3D(std::array<double, 9>& res, double angle_x, double angle_y, double angle_z) { Matrix3x3::GetRotation3D(res, Quaternion::CreateRotation(angle_x, angle_y, angle_z)); }
		static void GetRotation3D(Matrix3x3& res, const Quaternion& rotation) { Matrix3x3::GetRotation3D(res.data, rotation); }
		static void GetRotation3D(Matrix3x3& res, const Vector3& angles) { Matrix3x3::GetRotation3D(res.data, angles); }
		static void GetRotation3D(Matrix3x3& res, double angle_x, double angle_y, double angle_z) { Matrix3x3::GetRotation3D(res.data, angle_x, angle_y, angle_z); }
		static Matrix3x3 GetRotation3D(const Quaternion& rotation) {
			std::array<double, 9> _tmp;
			Matrix3x3::GetRotation3D(_tmp, rotation);
			return Matrix3x3(_tmp);
		}
		static Matrix3x3 GetRotation3D(const Vector3& angles) {
			std::array<double, 9> _tmp;
			Matrix3x3::GetRotation3D(_tmp, angles);
			return Matrix3x3(_tmp);
		}
		static Matrix3x3 GetRotation3D(double angle_x, double angle_y, double angle_z) {
			std::array<double, 9> _tmp;
			Matrix3x3::GetRotation3D(_tmp, angle_x, angle_y, angle_z);
			return Matrix3x3(_tmp);
		}
	};

	/// @brief 2x2 matrix with row-major storage.
	struct Matrix2x2 {
	private:
		std::array<double, 4>data;
	public:
		Matrix2x2() { data.fill(0); }
		Matrix2x2(double _fill) { data.fill(_fill); }
		Matrix2x2(const std::array<double, 4>& _data) : data(_data) {}

		Matrix2x2 operator-() const {
			std::array<double, 4> _tmp;
			for (int i = 0; i < 4; ++i) _tmp[i] = -data[i];
			return Matrix2x2(_tmp);
		}
		double& operator()(int r, int c) { return data[r * 2 + c]; }
		const double& operator()(int r, int c) const { return data[r * 2 + c]; }
		
		Matrix2x2& operator=(const Matrix2x2& _m) { data = _m.data; return *this; }
		Matrix2x2& operator=(const std::array<double, 4>& _m) { data = _m; return *this; }
		Matrix2x2& operator=(double _fill) { data.fill(_fill); return *this; }
		Matrix2x2& operator+=(const std::array<double, 4>& _m) { Matrix2x2::Add(data, _m, data); return *this; }
		Matrix2x2& operator+=(const Matrix2x2& _m) { Matrix2x2::Add(data, _m.data, data); return *this; }
		Matrix2x2& operator+=(double _v) {
			for (double& v : data) v += _v;
			return *this;
		}
		Matrix2x2& operator-=(const std::array<double, 4>& _m) { Matrix2x2::Subtract(data, _m, data); return *this; }
		Matrix2x2& operator-=(const Matrix2x2& _m) { Matrix2x2::Subtract(data, _m.data, data); return *this; }
		Matrix2x2& operator-=(double _v) {
			for (double& v : data) v -= _v;
			return *this;
		}

		Matrix2x2& operator*=(double _s) {
			for (double& v : data) v *= _s;
			return *this;
		}
		Matrix2x2& operator*=(const std::array<double, 4>& _m) {
			std::array<double, 4> _tmp;
			Matrix2x2::Multiply(data, _m, _tmp);
			data.swap(_tmp);
			return *this;
		}
		Matrix2x2& operator*=(const Matrix2x2& _m) {
			std::array<double, 4> _tmp;
			Matrix2x2::Multiply(data, _m.data, _tmp);
			data.swap(_tmp);
			return *this;
		}

		Matrix2x2 operator+(const Matrix2x2& _m) const {
			std::array<double, 4> _tmp;
			Matrix2x2::Add(data, _m.data, _tmp);
			return Matrix2x2(_tmp);
		}
		Matrix2x2 operator+(const std::array<double, 4>& _m) const {
			std::array<double, 4> _tmp;
			Matrix2x2::Add(data, _m, _tmp);
			return Matrix2x2(_tmp);
		}
		Matrix2x2 operator+(double _v) const {
			std::array<double, 4> _tmp;
			for (int i = 0; i < 4; ++i) _tmp[i] = data[i] + _v;
			return Matrix2x2(_tmp);
		}
		friend Matrix2x2 operator+(const std::array<double, 4>& l, const Matrix2x2& r) {
			std::array<double, 4> _tmp;
			Matrix2x2::Add(l, r.data, _tmp);
			return Matrix2x2(_tmp);
		}
		friend Matrix2x2 operator+(double _v, const Matrix2x2& r) {
			std::array<double, 4> _tmp;
			for (int i = 0; i < 4; ++i) _tmp[i] = _v + r.data[i];
			return Matrix2x2(_tmp);
		}

		Matrix2x2 operator-(const Matrix2x2& _m) const {
			std::array<double, 4> _tmp;
			Matrix2x2::Subtract(data, _m.data, _tmp);
			return Matrix2x2(_tmp);
		}
		Matrix2x2 operator-(const std::array<double, 4>& _m) const {
			std::array<double, 4> _tmp;
			Matrix2x2::Subtract(data, _m, _tmp);
			return Matrix2x2(_tmp);
		}
		Matrix2x2 operator-(double _v) const {
			std::array<double, 4> _tmp;
			for (int i = 0; i < 4; ++i) _tmp[i] = data[i] - _v;
			return Matrix2x2(_tmp);
		}
		friend Matrix2x2 operator-(const std::array<double, 4>& l, const Matrix2x2& r) {
			std::array<double, 4> _tmp;
			Matrix2x2::Subtract(l, r.data, _tmp);
			return Matrix2x2(_tmp);
		}
		friend Matrix2x2 operator-(double _v, const Matrix2x2& r) {
			std::array<double, 4> _tmp;
			for (int i = 0; i < 4; ++i) _tmp[i] = _v - r.data[i];
			return Matrix2x2(_tmp);
		}

		Matrix2x2 operator*(const Matrix2x2& _m) const {
			std::array<double, 4> _tmp;
			Matrix2x2::Multiply(data, _m.data, _tmp);
			return Matrix2x2(_tmp);
		}
		Matrix2x2 operator*(const std::array<double, 4>& _m) const {
			std::array<double, 4> _tmp;
			Matrix2x2::Multiply(data, _m, _tmp);
			return Matrix2x2(_tmp);
		}
		Matrix2x2 operator*(double _v) const {
			std::array<double, 4> _tmp;
			for (int i = 0; i < 4; ++i) _tmp[i] = data[i] * _v;
			return Matrix2x2(_tmp);
		}
		friend Matrix2x2 operator*(const std::array<double, 4>& l, const Matrix2x2& r) {
			std::array<double, 4> _tmp;
			Matrix2x2::Multiply(l, r.data, _tmp);
			return Matrix2x2(_tmp);
		}
		friend Matrix2x2 operator*(double _v, const Matrix2x2& r) {
			std::array<double, 4> _tmp;
			for (int i = 0; i < 4; ++i) _tmp[i] = _v * r.data[i];
			return Matrix2x2(_tmp);
		}

		Vector2 operator*(const Vector2& _v) const {
			return Vector2(
				data[0] * _v.x + data[1] * _v.y,
				data[2] * _v.x + data[3] * _v.y
			);
		}

		double& operator[](int i) { return data[i]; }
		const double& operator[](int i) const { return data[i]; }

		std::array<double, 4>& Data() { return data; }
		const std::array<double, 4>& Data() const { return data; }

		static void Add(const std::array<double, 4>& l, const std::array<double, 4>& r, std::array<double, 4>& res) {
			std::transform(l.cbegin(), l.cend(), r.cbegin(), res.begin(), std::plus<double>());
		}
		static void Add(const Matrix2x2& l, const Matrix2x2& r, Matrix2x2& res) { Matrix2x2::Add(l.data, r.data, res.data); }

		static void Subtract(const std::array<double, 4>& l, const std::array<double, 4>& r, std::array<double, 4>& res) {
			std::transform(l.cbegin(), l.cend(), r.cbegin(), res.begin(), std::minus<double>());
		}
		static void Subtract(const Matrix2x2& l, const Matrix2x2& r, Matrix2x2& res) { Matrix2x2::Subtract(l.data, r.data, res.data); }

		static void Multiply(const std::array<double, 4>& l, const std::array<double, 4>& r, std::array<double, 4>& res) {
			auto res_ptr = res.begin();
			for (int _r = 0; _r < 2; ++_r) {
				for (int _c = 0; _c < 2; ++_c, ++res_ptr)
					(*res_ptr) = (l[_r * 2] * r[_c]) + (l[_r * 2 + 1] * r[_c + 2]);
			}
		}
		static void Multiply(const Matrix2x2& l, const Matrix2x2& r, Matrix2x2& res) { Matrix2x2::Multiply(l.data, r.data, res.data); }

		static void GetIdentity(std::array<double, 4>& res, double scalar = 1.0) {
			std::array<double, 4> _tmp = {scalar, 0,
										  0, scalar};
			res.swap(_tmp);
		}
		static void GetIdentity(Matrix2x2& res, double scalar = 1.0) { Matrix2x2::GetIdentity(res.data, scalar); }
		static Matrix2x2 GetIdentity(double scalar = 1.0) {
			std::array<double, 4> _tmp;
			Matrix2x2::GetIdentity(_tmp, scalar);
			return Matrix2x2(_tmp);
		}

		static void GetScale(std::array<double, 4>& res, const Vector2& amount) {
			std::array<double, 4> _tmp = {amount.x, 0,
						       			  0, amount.y};
			res.swap(_tmp);
		}
		static void GetScale(Matrix2x2& res, const Vector2& amount) { Matrix2x2::GetScale(res.data, amount); }
		static Matrix2x2 GetScale(const Vector2& amount) {
			std::array<double, 4> _tmp;
			Matrix2x2::GetScale(_tmp, amount);
			return Matrix2x2(_tmp);
		}

		static void GetRotation(std::array<double, 4>& res, double angle) {
			double s = std::sin(angle), c = std::cos(angle);
			std::array<double, 4> _tmp = {c, -s,
										  s, c};
			res.swap(_tmp);
		}
		static void GetRotation(Matrix2x2& res, double angle) { Matrix2x2::GetRotation(res.data, angle); }
		static Matrix2x2 GetRotation(double angle) {
			std::array<double, 4> _tmp;
			Matrix2x2::GetRotation(_tmp, angle);
			return Matrix2x2(_tmp);
		}
	};
}