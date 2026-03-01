#include "Engine/Engine_Matrix.h"
#include "Engine/Engine_Quaternion.h"

namespace Engine {
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

    void Matrix4x4::GetTransformation3D(std::array<double, 16>& res, const Vector3& translation, const Vector3& rotation, const Vector3& scale) {
        Matrix4x4::GetRotation3D(res, rotation);

        res[0] *= scale.x; res[1] *= scale.y; res[2] *= scale.z;
        res[4] *= scale.x; res[5] *= scale.y; res[6] *= scale.z;
        res[8] *= scale.x; res[9] *= scale.y; res[10] *= scale.z;

        res[3] = translation.x; res[7] = translation.y; res[11] = translation.z;
    }

    void Matrix4x4::GetFrustumProjection(std::array<double, 16>& res, double left, double right, double top, double bottom, double near, double far) {
        if (!std::isfinite(left) || !std::isfinite(right) ||
            !std::isfinite(top) || !std::isfinite(bottom) ||
            !std::isfinite(near) || !std::isfinite(far)) {
            Matrix4x4::GetIdentity(res, 1);
            return;
        }

        double x_factor = right - left;
        double y_factor = top - bottom;
        double z_factor = far - near;

        if (NumericConstants::IsNearZero(x_factor) ||
            NumericConstants::IsNearZero(y_factor) ||
            NumericConstants::IsNearZero(z_factor)) {
            Matrix4x4::GetIdentity(res, 1);
            return;
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

    void Matrix4x4::GetPerspectiveProjection(std::array<double, 16>& res, double fovY, double aspect, double near, double far) {
        static const double pi = std::acos(-1);

        if (!std::isfinite(fovY) || !std::isfinite(aspect) ||
            !std::isfinite(near) || !std::isfinite(far) ||
            (near >= far)) {
            Matrix4x4::GetIdentity(res, 1);
            return;
        }

        if (NumericConstants::IsNearZero(std::remainder(fovY, pi))) {
            Matrix4x4::GetIdentity(res, 1);
            return;
        }

        double s = 1.0 / std::tan(fovY * 0.5);
        double z_factor = far - near;

        if (NumericConstants::IsNearZero(z_factor) || NumericConstants::IsNearZeroOrNegative(aspect)) {
            Matrix4x4::GetIdentity(res, 1);
            return;
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

    void Matrix4x4::GetOrthographicProjection(std::array<double, 16>& res, double left, double right, double top, double bottom, double near, double far) {
        if (!std::isfinite(left) || !std::isfinite(right) ||
            !std::isfinite(top) || !std::isfinite(bottom) ||
            !std::isfinite(near) || !std::isfinite(far)) {
            Matrix4x4::GetIdentity(res, 1);
            return;
        }

        double x_factor = right - left;
        double y_factor = top - bottom;
        double z_factor = far - near;

        if (NumericConstants::IsNearZero(x_factor) ||
            NumericConstants::IsNearZero(y_factor) ||
            NumericConstants::IsNearZero(z_factor)) {
            Matrix4x4::GetIdentity(res, 1);
            return;
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

    void Matrix3x3::GetTransformation2D(std::array<double, 9>& res, const Vector3& translation, double rotation_angle, const Vector3& scale) {
        Matrix3x3::GetRotation2D(res, rotation_angle);

        res[0] *= scale.x; res[1] *= scale.y;
        res[3] *= scale.x; res[4] *= scale.y;

        res[2] = translation.x; res[5] = translation.y;
    }

    void Matrix3x3::GetViewportTransform2D(std::array<double, 9>& res, double left_i, double right_i, double top_i, double bottom_i, double left_o, double right_o, double top_o, double bottom_o) {
        if (NumericConstants::IsNearZero(right_i - left_i) || NumericConstants::IsNearZero(top_i - bottom_i)) {
            Matrix3x3::GetIdentity(res, 1);
            return;
        }

        double x_factor = 1.0 / (right_i - left_i);
        double y_factor = 1.0 / (top_i - bottom_i);

        std::array<double, 9> out = {
            (right_o - left_o) * x_factor, 0, (right_i * left_o - right_o * left_i) * x_factor,
            0, (top_o - bottom_o) * y_factor, (top_i * bottom_o - top_o * bottom_i) * y_factor,
            0, 0, 1
        };
        res.swap(out);
    }
}
