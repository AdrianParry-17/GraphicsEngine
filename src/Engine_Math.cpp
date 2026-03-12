#include "Engine/Engine_Quaternion.h"
#include "Engine/Engine_Transform.h"

namespace Engine {

    // -------------------------------------------------------------------------
    // Quaternion
    // -------------------------------------------------------------------------

    Quaternion& Quaternion::operator*=(const Quaternion& _q) {
        double _a = a * _q.a - b * _q.b - c * _q.c - d * _q.d;
        double _b = a * _q.b + b * _q.a + c * _q.d - d * _q.c;
        double _c = a * _q.c - b * _q.d + c * _q.a + d * _q.b;
        double _d = a * _q.d + b * _q.c - c * _q.b + d * _q.a;
        a = _a; b = _b; c = _c; d = _d;
        return *this;
    }

    Quaternion Quaternion::operator*(const Quaternion& _q) const {
        return Quaternion(
            a * _q.a - b * _q.b - c * _q.c - d * _q.d,
            a * _q.b + b * _q.a + c * _q.d - d * _q.c,
            a * _q.c - b * _q.d + c * _q.a + d * _q.b,
            a * _q.d + b * _q.c - c * _q.b + d * _q.a
        );
    }

    double Quaternion::Norm() const {
        return std::hypot(std::hypot(a, b), std::hypot(c, d));
    }

    Quaternion Quaternion::Unit() const {
        double factor = Norm();
        if (NumericConstants::IsNearZero(factor))
            return Quaternion(0, 0, 0, 0);
        factor = 1.0 / factor;
        return Quaternion(a * factor, b * factor, c * factor, d * factor);
    }

    Quaternion Quaternion::Reciprocal() const {
        double factor = Norm();
        if (NumericConstants::IsNearZero(factor))
            return Quaternion(0, 0, 0, 0);
        factor = 1.0 / (factor * factor);
        return Quaternion(a * factor, -b * factor, -c * factor, -d * factor);
    }

    Quaternion Quaternion::CreateRotation(double rx, double ry, double rz) {
        double cx = std::cos(0.5 * rx), sx = std::sin(0.5 * rx);
        double cy = std::cos(0.5 * ry), sy = std::sin(0.5 * ry);
        double cz = std::cos(0.5 * rz), sz = std::sin(0.5 * rz);
        return Quaternion(
            cx * cy * cz + sx * sy * sz,
            sx * cy * cz - cx * sy * sz,
            cx * sy * cz + sx * cy * sz,
            cx * cy * sz - sx * sy * cz
        );
    }

    // -------------------------------------------------------------------------
    // Transform2D
    // -------------------------------------------------------------------------

    void Transform2D::GetScale2x2(std::array<double, 4>& res, double x, double y) {
        std::array<double, 4> _tmp = {x, 0,
                                      0, y};
        res.swap(_tmp);
    }

    void Transform2D::GetRotation2x2(std::array<double, 4>& res, double angle) {
        double s = std::sin(angle), c = std::cos(angle);
        std::array<double, 4> _tmp = {c, -s,
                                      s,  c};
        res.swap(_tmp);
    }

    void Transform2D::GetTranslation(std::array<double, 9>& res, double x, double y) {
        std::array<double, 9> _tmp = {1, 0, x,
                                      0, 1, y,
                                      0, 0, 1};
        res.swap(_tmp);
    }

    void Transform2D::GetRotation(std::array<double, 9>& res, double angle) {
        double s = std::sin(angle), c = std::cos(angle);
        std::array<double, 9> _tmp = {c, -s, 0,
                                      s,  c, 0,
                                      0,  0, 1};
        res.swap(_tmp);
    }

    void Transform2D::GetScale(std::array<double, 9>& res, double x, double y) {
        std::array<double, 9> _tmp = {x, 0, 0,
                                      0, y, 0,
                                      0, 0, 1};
        res.swap(_tmp);
    }

    void Transform2D::GetTransformation(std::array<double, 9>& res, const Vector3& translation, double rotation_angle, const Vector3& scale) {
        Transform2D::GetRotation(res, rotation_angle);
        res[0] *= scale.x(); res[1] *= scale.y();
        res[3] *= scale.x(); res[4] *= scale.y();
        res[2] = translation.x(); res[5] = translation.y();
    }

    void Transform2D::GetViewportTransform(std::array<double, 9>& res, double left_i, double right_i, double top_i, double bottom_i, double left_o, double right_o, double top_o, double bottom_o) {
        if (NumericConstants::IsNearZero(right_i - left_i) || NumericConstants::IsNearZero(top_i - bottom_i)) {
            Matrix3x3::GetIdentity(res, 1); return;
        }
        double x_factor = 1.0 / (right_i - left_i), y_factor = 1.0 / (top_i - bottom_i);
        std::array<double, 9> _tmp = {
            (right_o - left_o) * x_factor, 0, (right_i * left_o - right_o * left_i) * x_factor,
            0, (top_o - bottom_o) * y_factor, (top_i * bottom_o - top_o * bottom_i) * y_factor,
            0, 0, 1
        };
        res.swap(_tmp);
    }

    // -------------------------------------------------------------------------
    // Transform3D
    // -------------------------------------------------------------------------

    void Transform3D::GetScale3x3(std::array<double, 9>& res, double x, double y, double z) {
        std::array<double, 9> _tmp = {x, 0, 0,
                                      0, y, 0,
                                      0, 0, z};
        res.swap(_tmp);
    }

    void Transform3D::GetRotation3x3(std::array<double, 9>& res, const Quaternion& rotation) {
        Quaternion q = rotation.Unit();
        double bb = q.b * q.b, cc = q.c * q.c, dd = q.d * q.d;
        double bc = q.b * q.c, bd = q.b * q.d, cd = q.c * q.d;
        double ad = q.a * q.d, ac = q.a * q.c, ab = q.a * q.b;
        std::array<double, 9> _tmp = {
            1 - 2 * (cc + dd), 2 * (bc - ad),     2 * (bd + ac),
            2 * (bc + ad),     1 - 2 * (bb + dd), 2 * (cd - ab),
            2 * (bd - ac),     2 * (cd + ab),     1 - 2 * (bb + cc)
        };
        res.swap(_tmp);
    }

    void Transform3D::GetTranslation(std::array<double, 16>& res, double x, double y, double z) {
        std::array<double, 16> _tmp = {1, 0, 0, x,
                                       0, 1, 0, y,
                                       0, 0, 1, z,
                                       0, 0, 0, 1};
        res.swap(_tmp);
    }

    void Transform3D::GetScale(std::array<double, 16>& res, double x, double y, double z) {
        std::array<double, 16> _tmp = {x, 0, 0, 0,
                                       0, y, 0, 0,
                                       0, 0, z, 0,
                                       0, 0, 0, 1};
        res.swap(_tmp);
    }

    void Transform3D::GetRotation(std::array<double, 16>& res, const Quaternion& rotation) {
        Quaternion q = rotation.Unit();
        double bb = q.b * q.b, cc = q.c * q.c, dd = q.d * q.d;
        double bc = q.b * q.c, bd = q.b * q.d, cd = q.c * q.d;
        double ad = q.a * q.d, ac = q.a * q.c, ab = q.a * q.b;
        std::array<double, 16> _tmp = {
            1 - 2 * (cc + dd), 2 * (bc - ad),     2 * (bd + ac),     0,
            2 * (bc + ad),     1 - 2 * (bb + dd), 2 * (cd - ab),     0,
            2 * (bd - ac),     2 * (cd + ab),     1 - 2 * (bb + cc), 0,
            0, 0, 0, 1
        };
        res.swap(_tmp);
    }

    void Transform3D::GetTransformation(std::array<double, 16>& res, const Vector3& translation, const Vector3& rotation, const Vector3& scale) {
        Transform3D::GetRotation(res, rotation);
        res[0] *= scale.x(); res[1] *= scale.y(); res[2]  *= scale.z();
        res[4] *= scale.x(); res[5] *= scale.y(); res[6]  *= scale.z();
        res[8] *= scale.x(); res[9] *= scale.y(); res[10] *= scale.z();
        res[3] = translation.x(); res[7] = translation.y(); res[11] = translation.z();
    }

    void Transform3D::GetFrustumProjection(std::array<double, 16>& res, double left, double right, double top, double bottom, double near, double far) {
        if (!std::isfinite(left) || !std::isfinite(right)  ||
            !std::isfinite(top)  || !std::isfinite(bottom) ||
            !std::isfinite(near) || !std::isfinite(far)) {
            Matrix4x4::GetIdentity(res, 1); return;
        }
        double x_factor = right - left;
        double y_factor = top - bottom;
        double z_factor = far - near;
        if (NumericConstants::IsNearZero(x_factor) ||
            NumericConstants::IsNearZero(y_factor) ||
            NumericConstants::IsNearZero(z_factor)) {
            Matrix4x4::GetIdentity(res, 1); return;
        }
        x_factor = 1.0 / x_factor;
        y_factor = 1.0 / y_factor;
        z_factor = 1.0 / z_factor;
        std::array<double, 16> out = {
            2 * near * x_factor, 0, (right + left) * x_factor, 0,
            0, 2 * near * y_factor, (top + bottom) * y_factor, 0,
            0, 0, -(far + near) * z_factor, -2 * far * near * z_factor,
            0, 0, -1, 0
        };
        res.swap(out);
    }

    void Transform3D::GetPerspectiveProjection(std::array<double, 16>& res, double fovY, double aspect, double near, double far) {
        static const double pi = std::acos(-1.0);
        if (!std::isfinite(fovY) || !std::isfinite(aspect) ||
            !std::isfinite(near) || !std::isfinite(far) ||
            (near >= far)) {
            Matrix4x4::GetIdentity(res, 1); return;
        }
        if (NumericConstants::IsNearZero(std::remainder(fovY, pi))) {
            Matrix4x4::GetIdentity(res, 1); return;
        }
        double s = 1.0 / std::tan(fovY * 0.5);
        double z_factor = far - near;
        if (NumericConstants::IsNearZero(z_factor) || NumericConstants::IsNearZeroOrNegative(aspect)) {
            Matrix4x4::GetIdentity(res, 1); return;
        }
        z_factor = 1.0 / z_factor;
        std::array<double, 16> out = {
            s / aspect, 0, 0, 0,
            0, s, 0, 0,
            0, 0, -(far + near) * z_factor, -2 * far * near * z_factor,
            0, 0, -1, 0
        };
        res.swap(out);
    }

    void Transform3D::GetOrthographicProjection(std::array<double, 16>& res, double left, double right, double top, double bottom, double near, double far) {
        if (!std::isfinite(left) || !std::isfinite(right)  ||
            !std::isfinite(top)  || !std::isfinite(bottom) ||
            !std::isfinite(near) || !std::isfinite(far)) {
            Matrix4x4::GetIdentity(res, 1); return;
        }
        double x_factor = right - left;
        double y_factor = top - bottom;
        double z_factor = far - near;
        if (NumericConstants::IsNearZero(x_factor) ||
            NumericConstants::IsNearZero(y_factor) ||
            NumericConstants::IsNearZero(z_factor)) {
            Matrix4x4::GetIdentity(res, 1); return;
        }
        x_factor = 1.0 / x_factor;
        y_factor = 1.0 / y_factor;
        z_factor = 1.0 / z_factor;
        std::array<double, 16> out = {
            2 * x_factor, 0, 0, -(right + left) * x_factor,
            0, 2 * y_factor, 0, -(top + bottom) * y_factor,
            0, 0, -2 * z_factor, -(far + near) * z_factor,
            0, 0, 0, 1
        };
        res.swap(out);
    }

} // namespace Engine