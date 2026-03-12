#pragma once

#include "Engine_Matrix.h"
#include "Engine_WorldGraphics.h"
#include "Engine_Quaternion.h"

namespace Engine {
    /**
     * @brief Static utility class for 2D transformation matrices.
     *
     * Provides factory methods that construct 2×2 and 3×3 matrices for
     * 2D scale, rotation, translation, combined transformations, and
     * viewport mapping. All methods are static; the class is non-instantiable.
     */
    class Transform2D final {
    public:
        /**
         * @brief Fills a raw 4-element array with a 2×2 scale matrix.
         * @param res Output array (row-major 2×2).
         * @param x   Scale factor along X.
         * @param y   Scale factor along Y.
         */
        static void GetScale2x2(std::array<double, 4>& res, double x, double y);
        /** @brief Fills a Matrix2x2 with a 2×2 scale matrix. @param res Output matrix. @param x Scale X. @param y Scale Y. */
        static void GetScale2x2(Matrix2x2& res, double x, double y) { Transform2D::GetScale2x2(res.Data(), x, y); }
        /** @brief Fills a raw array with a 2×2 scale matrix from a Vector2. @param res Output array. @param amount Scale vector. */
        static void GetScale2x2(std::array<double, 4>& res, const Vector2& amount) { Transform2D::GetScale2x2(res, amount.x(), amount.y()); }
        /** @brief Fills a Matrix2x2 with a 2×2 scale matrix from a Vector2. @param res Output matrix. @param amount Scale vector. */
        static void GetScale2x2(Matrix2x2& res, const Vector2& amount) { Transform2D::GetScale2x2(res.Data(), amount.x(), amount.y()); }
        /**
         * @brief Returns a 2×2 scale matrix.
         * @param x Scale factor along X.
         * @param y Scale factor along Y.
         * @return Constructed Matrix2x2.
         */
        static Matrix2x2 GetScale2x2(double x, double y) {
            std::array<double, 4> _res; Transform2D::GetScale2x2(_res, x, y); return _res;
        }
        /**
         * @brief Returns a 2×2 scale matrix from a Vector2.
         * @param amount Scale vector.
         * @return Constructed Matrix2x2.
         */
        static Matrix2x2 GetScale2x2(const Vector2& amount) {
            std::array<double, 4> _res; Transform2D::GetScale2x2(_res, amount); return _res;
        }

        /**
         * @brief Fills a raw 4-element array with a 2×2 rotation matrix.
         * @param res   Output array (row-major 2×2).
         * @param angle Rotation angle in radians.
         */
        static void GetRotation2x2(std::array<double, 4>& res, double angle);
        /** @brief Fills a Matrix2x2 with a 2×2 rotation matrix. @param res Output matrix. @param angle Angle in radians. */
        static void GetRotation2x2(Matrix2x2& res, double angle) { Transform2D::GetRotation2x2(res.Data(), angle); }
        /**
         * @brief Returns a 2×2 rotation matrix.
         * @param angle Rotation angle in radians.
         * @return Constructed Matrix2x2.
         */
        static Matrix2x2 GetRotation2x2(double angle) {
            std::array<double, 4> _res; Transform2D::GetRotation2x2(_res, angle); return _res;
        }

        /**
         * @brief Fills a raw 9-element array with a 3×3 translation matrix (homogeneous 2D).
         * @param res Output array (row-major 3×3).
         * @param x   Translation along X.
         * @param y   Translation along Y.
         */
        static void GetTranslation(std::array<double, 9>& res, double x, double y);
        /** @brief Fills a Matrix3x3 with a 2D translation matrix. @param res Output matrix. @param x Translate X. @param y Translate Y. */
        static void GetTranslation(Matrix3x3& res, double x, double y) { Transform2D::GetTranslation(res.Data(), x, y); }
        /** @brief Fills a raw array with a 2D translation matrix from a Vector2. @param res Output array. @param amount Translation vector. */
        static void GetTranslation(std::array<double, 9>& res, const Vector2& amount) { Transform2D::GetTranslation(res, amount.x(), amount.y()); }
        /** @brief Fills a Matrix3x3 with a 2D translation matrix from a Vector2. @param res Output matrix. @param amount Translation vector. */
        static void GetTranslation(Matrix3x3& res, const Vector2& amount) { Transform2D::GetTranslation(res.Data(), amount.x(), amount.y()); }
        /**
         * @brief Returns a 3×3 homogeneous 2D translation matrix.
         * @param x Translation along X.
         * @param y Translation along Y.
         * @return Constructed Matrix3x3.
         */
        static Matrix3x3 GetTranslation(double x, double y) {
            std::array<double, 9> _res; Transform2D::GetTranslation(_res, x, y); return _res;
        }
        /**
         * @brief Returns a 3×3 homogeneous 2D translation matrix from a Vector2.
         * @param amount Translation vector.
         * @return Constructed Matrix3x3.
         */
        static Matrix3x3 GetTranslation(const Vector2& amount) {
            std::array<double, 9> _res; Transform2D::GetTranslation(_res, amount); return _res;
        }

        /**
         * @brief Fills a raw 9-element array with a 3×3 homogeneous 2D rotation matrix.
         * @param res   Output array (row-major 3×3).
         * @param angle Rotation angle in radians.
         */
        static void GetRotation(std::array<double, 9>& res, double angle);
        /** @brief Fills a Matrix3x3 with a 2D rotation matrix. @param res Output matrix. @param angle Angle in radians. */
        static void GetRotation(Matrix3x3& res, double angle) { Transform2D::GetRotation(res.Data(), angle); }
        /**
         * @brief Returns a 3×3 homogeneous 2D rotation matrix.
         * @param angle Rotation angle in radians.
         * @return Constructed Matrix3x3.
         */
        static Matrix3x3 GetRotation(double angle) {
            std::array<double, 9> _res; Transform2D::GetRotation(_res, angle); return _res;
        }

        /**
         * @brief Fills a raw 9-element array with a 3×3 homogeneous 2D scale matrix.
         * @param res Output array (row-major 3×3).
         * @param x   Scale factor along X.
         * @param y   Scale factor along Y.
         */
        static void GetScale(std::array<double, 9>& res, double x, double y);
        /** @brief Fills a Matrix3x3 with a 2D scale matrix. @param res Output matrix. @param x Scale X. @param y Scale Y. */
        static void GetScale(Matrix3x3& res, double x, double y) { Transform2D::GetScale(res.Data(), x, y); }
        /** @brief Fills a raw array with a 2D scale matrix from a Vector2. @param res Output array. @param amount Scale vector. */
        static void GetScale(std::array<double, 9>& res, const Vector2& amount) { Transform2D::GetScale(res, amount.x(), amount.y()); }
        /** @brief Fills a Matrix3x3 with a 2D scale matrix from a Vector2. @param res Output matrix. @param amount Scale vector. */
        static void GetScale(Matrix3x3& res, const Vector2& amount) { Transform2D::GetScale(res.Data(), amount.x(), amount.y()); }
        /**
         * @brief Returns a 3×3 homogeneous 2D scale matrix.
         * @param x Scale factor along X.
         * @param y Scale factor along Y.
         * @return Constructed Matrix3x3.
         */
        static Matrix3x3 GetScale(double x, double y) {
            std::array<double, 9> _res; Transform2D::GetScale(_res, x, y); return _res;
        }
        /**
         * @brief Returns a 3×3 homogeneous 2D scale matrix from a Vector2.
         * @param amount Scale vector.
         * @return Constructed Matrix3x3.
         */
        static Matrix3x3 GetScale(const Vector2& amount) {
            std::array<double, 9> _res; Transform2D::GetScale(_res, amount); return _res;
        }

        /**
         * @brief Fills a raw 9-element array with a combined 2D TRS matrix (T × R × S).
         * @param res            Output array (row-major 3×3).
         * @param translation    Translation as a Vector3 (x/y used).
         * @param rotation_angle Rotation angle in radians.
         * @param scale          Scale as a Vector3 (x/y used).
         */
        static void GetTransformation(std::array<double, 9>& res, const Vector3& translation, double rotation_angle, const Vector3& scale);
        /** @brief Fills a Matrix3x3 with a combined 2D TRS matrix. @param res Output matrix. @param translation Translation (x/y). @param rotation_angle Angle in radians. @param scale Scale (x/y). */
        static void GetTransformation(Matrix3x3& res, const Vector3& translation, double rotation_angle, const Vector3& scale) {
            Transform2D::GetTransformation(res.Data(), translation, rotation_angle, scale);
        }
        /**
         * @brief Returns a 3×3 combined 2D TRS matrix.
         * @param translation    Translation (x/y).
         * @param rotation_angle Rotation angle in radians.
         * @param scale          Scale (x/y).
         * @return Constructed Matrix3x3.
         */
        static Matrix3x3 GetTransformation(const Vector3& translation, double rotation_angle, const Vector3& scale) {
            std::array<double, 9> _tmp;
            Transform2D::GetTransformation(_tmp, translation, rotation_angle, scale);
            return _tmp;
        }

        /**
         * @brief Fills a raw 9-element array with a 2D viewport mapping matrix.
         *
         * Maps the rectangle [left_i, right_i] × [top_i, bottom_i] (input)
         * to [left_o, right_o] × [top_o, bottom_o] (output).
         * @param res      Output array (row-major 3×3).
         * @param left_i   Input left bound.
         * @param right_i  Input right bound.
         * @param top_i    Input top bound.
         * @param bottom_i Input bottom bound.
         * @param left_o   Output left bound.
         * @param right_o  Output right bound.
         * @param top_o    Output top bound.
         * @param bottom_o Output bottom bound.
         */
        static void GetViewportTransform(std::array<double, 9>& res, double left_i, double right_i, double top_i, double bottom_i, double left_o, double right_o, double top_o, double bottom_o);
        /** @brief Fills a Matrix3x3 with a 2D viewport mapping matrix. @param res Output matrix. @param left_i Input left. @param right_i Input right. @param top_i Input top. @param bottom_i Input bottom. @param left_o Output left. @param right_o Output right. @param top_o Output top. @param bottom_o Output bottom. */
        static void GetViewportTransform(Matrix3x3& res, double left_i, double right_i, double top_i, double bottom_i, double left_o, double right_o, double top_o, double bottom_o) {
            Transform2D::GetViewportTransform(res.Data(), left_i, right_i, top_i, bottom_i, left_o, right_o, top_o, bottom_o);
        }
        /**
         * @brief Returns a 3×3 2D viewport mapping matrix.
         * @param left_i Input left. @param right_i Input right. @param top_i Input top. @param bottom_i Input bottom.
         * @param left_o Output left. @param right_o Output right. @param top_o Output top. @param bottom_o Output bottom.
         * @return Constructed Matrix3x3.
         */
        static Matrix3x3 GetViewportTransform(double left_i, double right_i, double top_i, double bottom_i, double left_o, double right_o, double top_o, double bottom_o) {
            std::array<double, 9> _tmp;
            Transform2D::GetViewportTransform(_tmp, left_i, right_i, top_i, bottom_i, left_o, right_o, top_o, bottom_o);
            return Matrix3x3(_tmp);
        }
        /** @brief Fills a raw array with a viewport mapping matrix from Range objects. @param res Output array. @param x_in Input X range. @param y_in Input Y range. @param x_out Output X range. @param y_out Output Y range. */
        static void GetViewportTransform(std::array<double, 9>& res, const Range& x_in, const Range& y_in, const Range& x_out, const Range& y_out) {
            Transform2D::GetViewportTransform(res, x_in.start, x_in.end, y_in.start, y_in.end, x_out.start, x_out.end, y_out.start, y_out.end);
        }
        /** @brief Fills a Matrix3x3 with a viewport mapping matrix from Range objects. @param res Output matrix. @param x_in Input X range. @param y_in Input Y range. @param x_out Output X range. @param y_out Output Y range. */
        static void GetViewportTransform(Matrix3x3& res, const Range& x_in, const Range& y_in, const Range& x_out, const Range& y_out) {
            Transform2D::GetViewportTransform(res.Data(), x_in, y_in, x_out, y_out);
        }
        /**
         * @brief Returns a 3×3 viewport mapping matrix from Range objects.
         * @param x_in  Input X range.
         * @param y_in  Input Y range.
         * @param x_out Output X range.
         * @param y_out Output Y range.
         * @return Constructed Matrix3x3.
         */
        static Matrix3x3 GetViewportTransform(const Range& x_in, const Range& y_in, const Range& x_out, const Range& y_out) {
            std::array<double, 9> _tmp;
            Transform2D::GetViewportTransform(_tmp, x_in, y_in, x_out, y_out);
            return Matrix3x3(_tmp);
        }
    };

    /**
     * @brief Static utility class for 3D transformation matrices.
     *
     * Provides factory methods that construct 3×3 and 4×4 matrices for
     * 3D scale, rotation (via Quaternion or Euler angles), translation,
     * combined TRS transformations, and standard projection matrices
     * (frustum, perspective, orthographic). All methods are static;
     * the class is non-instantiable.
     */
    class Transform3D final {
    public:
        /**
         * @brief Fills a raw 9-element array with a 3×3 scale matrix.
         * @param res Output array (row-major 3×3).
         * @param x   Scale factor along X.
         * @param y   Scale factor along Y.
         * @param z   Scale factor along Z.
         */
        static void GetScale3x3(std::array<double, 9>& res, double x, double y, double z);
        /** @brief Fills a Matrix3x3 with a 3×3 scale matrix. @param res Output matrix. @param x Scale X. @param y Scale Y. @param z Scale Z. */
        static void GetScale3x3(Matrix3x3& res, double x, double y, double z) { Transform3D::GetScale3x3(res.Data(), x, y, z); }
        /** @brief Fills a raw array with a 3×3 scale matrix from a Vector3. @param res Output array. @param amount Scale vector. */
        static void GetScale3x3(std::array<double, 9>& res, const Vector3& amount) { Transform3D::GetScale3x3(res, amount.x(), amount.y(), amount.z()); }
        /** @brief Fills a Matrix3x3 with a 3×3 scale matrix from a Vector3. @param res Output matrix. @param amount Scale vector. */
        static void GetScale3x3(Matrix3x3& res, const Vector3& amount) { Transform3D::GetScale3x3(res.Data(), amount.x(), amount.y(), amount.z()); }
        /**
         * @brief Returns a 3×3 scale matrix.
         * @param x Scale X. @param y Scale Y. @param z Scale Z.
         * @return Constructed Matrix3x3.
         */
        static Matrix3x3 GetScale3x3(double x, double y, double z) {
            std::array<double, 9> _res; Transform3D::GetScale3x3(_res, x, y, z); return _res;
        }
        /**
         * @brief Returns a 3×3 scale matrix from a Vector3.
         * @param amount Scale vector.
         * @return Constructed Matrix3x3.
         */
        static Matrix3x3 GetScale3x3(const Vector3& amount) {
            std::array<double, 9> _res; Transform3D::GetScale3x3(_res, amount); return _res;
        }

        /**
         * @brief Fills a raw 9-element array with a 3×3 rotation matrix from a Quaternion.
         * @param res      Output array (row-major 3×3).
         * @param rotation Source quaternion.
         */
        static void GetRotation3x3(std::array<double, 9>& res, const Quaternion& rotation);
        /** @brief Fills a Matrix3x3 with a rotation matrix from a Quaternion. @param res Output matrix. @param rotation Source quaternion. */
        static void GetRotation3x3(Matrix3x3& res, const Quaternion& rotation) { Transform3D::GetRotation3x3(res.Data(), rotation); }
        /** @brief Fills a raw array with a rotation matrix from Euler angles. @param res Output array. @param x Pitch (radians). @param y Yaw (radians). @param z Roll (radians). */
        static void GetRotation3x3(std::array<double, 9>& res, double x, double y, double z) {
            Transform3D::GetRotation3x3(res, Quaternion::CreateRotation(x, y, z));
        }
        /** @brief Fills a Matrix3x3 with a rotation matrix from Euler angles. @param res Output matrix. @param x Pitch. @param y Yaw. @param z Roll. */
        static void GetRotation3x3(Matrix3x3& res, double x, double y, double z) { Transform3D::GetRotation3x3(res.Data(), x, y, z); }
        /** @brief Fills a raw array with a rotation matrix from a Vector3 of Euler angles. @param res Output array. @param amount Euler angles vector. */
        static void GetRotation3x3(std::array<double, 9>& res, const Vector3& amount) { Transform3D::GetRotation3x3(res, amount.x(), amount.y(), amount.z()); }
        /** @brief Fills a Matrix3x3 with a rotation matrix from a Vector3 of Euler angles. @param res Output matrix. @param amount Euler angles vector. */
        static void GetRotation3x3(Matrix3x3& res, const Vector3& amount) { Transform3D::GetRotation3x3(res.Data(), amount.x(), amount.y(), amount.z()); }
        /**
         * @brief Returns a 3×3 rotation matrix from Euler angles.
         * @param x Pitch (radians). @param y Yaw (radians). @param z Roll (radians).
         * @return Constructed Matrix3x3.
         */
        static Matrix3x3 GetRotation3x3(double x, double y, double z) {
            std::array<double, 9> _res; Transform3D::GetRotation3x3(_res, x, y, z); return _res;
        }
        /**
         * @brief Returns a 3×3 rotation matrix from a Vector3 of Euler angles.
         * @param amount Euler angles vector.
         * @return Constructed Matrix3x3.
         */
        static Matrix3x3 GetRotation3x3(const Vector3& amount) {
            std::array<double, 9> _res; Transform3D::GetRotation3x3(_res, amount); return _res;
        }
        /**
         * @brief Returns a 3×3 rotation matrix from a Quaternion.
         * @param rotation Source quaternion.
         * @return Constructed Matrix3x3.
         */
        static Matrix3x3 GetRotation3x3(const Quaternion& rotation) {
            std::array<double, 9> _res; Transform3D::GetRotation3x3(_res, rotation); return _res;
        }

        /**
         * @brief Fills a raw 16-element array with a 4×4 homogeneous translation matrix.
         * @param res Output array (row-major 4×4).
         * @param x   Translation along X.
         * @param y   Translation along Y.
         * @param z   Translation along Z.
         */
        static void GetTranslation(std::array<double, 16>& res, double x, double y, double z);
        /** @brief Fills a Matrix4x4 with a translation matrix. @param res Output matrix. @param x Translate X. @param y Translate Y. @param z Translate Z. */
        static void GetTranslation(Matrix4x4& res, double x, double y, double z) { Transform3D::GetTranslation(res.Data(), x, y, z); }
        /** @brief Fills a raw array with a translation matrix from a Vector3. @param res Output array. @param amount Translation vector. */
        static void GetTranslation(std::array<double, 16>& res, const Vector3& amount) { Transform3D::GetTranslation(res, amount.x(), amount.y(), amount.z()); }
        /** @brief Fills a Matrix4x4 with a translation matrix from a Vector3. @param res Output matrix. @param amount Translation vector. */
        static void GetTranslation(Matrix4x4& res, const Vector3& amount) { Transform3D::GetTranslation(res.Data(), amount.x(), amount.y(), amount.z()); }
        /**
         * @brief Returns a 4×4 homogeneous translation matrix.
         * @param x Translate X. @param y Translate Y. @param z Translate Z.
         * @return Constructed Matrix4x4.
         */
        static Matrix4x4 GetTranslation(double x, double y, double z) {
            std::array<double, 16> _res; Transform3D::GetTranslation(_res, x, y, z); return _res;
        }
        /**
         * @brief Returns a 4×4 homogeneous translation matrix from a Vector3.
         * @param amount Translation vector.
         * @return Constructed Matrix4x4.
         */
        static Matrix4x4 GetTranslation(const Vector3& amount) {
            std::array<double, 16> _res; Transform3D::GetTranslation(_res, amount); return _res;
        }

        /**
         * @brief Fills a raw 16-element array with a 4×4 homogeneous scale matrix.
         * @param res Output array (row-major 4×4).
         * @param x   Scale factor along X.
         * @param y   Scale factor along Y.
         * @param z   Scale factor along Z.
         */
        static void GetScale(std::array<double, 16>& res, double x, double y, double z);
        /** @brief Fills a Matrix4x4 with a scale matrix. @param res Output matrix. @param x Scale X. @param y Scale Y. @param z Scale Z. */
        static void GetScale(Matrix4x4& res, double x, double y, double z) { Transform3D::GetScale(res.Data(), x, y, z); }
        /** @brief Fills a raw array with a scale matrix from a Vector3. @param res Output array. @param amount Scale vector. */
        static void GetScale(std::array<double, 16>& res, const Vector3& amount) { Transform3D::GetScale(res, amount.x(), amount.y(), amount.z()); }
        /** @brief Fills a Matrix4x4 with a scale matrix from a Vector3. @param res Output matrix. @param amount Scale vector. */
        static void GetScale(Matrix4x4& res, const Vector3& amount) { Transform3D::GetScale(res.Data(), amount.x(), amount.y(), amount.z()); }
        /**
         * @brief Returns a 4×4 homogeneous scale matrix.
         * @param x Scale X. @param y Scale Y. @param z Scale Z.
         * @return Constructed Matrix4x4.
         */
        static Matrix4x4 GetScale(double x, double y, double z) {
            std::array<double, 16> _res; Transform3D::GetScale(_res, x, y, z); return _res;
        }
        /**
         * @brief Returns a 4×4 homogeneous scale matrix from a Vector3.
         * @param amount Scale vector.
         * @return Constructed Matrix4x4.
         */
        static Matrix4x4 GetScale(const Vector3& amount) {
            std::array<double, 16> _res; Transform3D::GetScale(_res, amount); return _res;
        }

        /**
         * @brief Fills a raw 16-element array with a 4×4 rotation matrix from a Quaternion.
         * @param res      Output array (row-major 4×4).
         * @param rotation Source quaternion.
         */
        static void GetRotation(std::array<double, 16>& res, const Quaternion& rotation);
        /** @brief Fills a Matrix4x4 with a rotation matrix from a Quaternion. @param res Output matrix. @param rotation Source quaternion. */
        static void GetRotation(Matrix4x4& res, const Quaternion& rotation) { Transform3D::GetRotation(res.Data(), rotation); }
        /** @brief Fills a raw array with a rotation matrix from Euler angles. @param res Output array. @param x Pitch. @param y Yaw. @param z Roll. */
        static void GetRotation(std::array<double, 16>& res, double x, double y, double z) {
            Transform3D::GetRotation(res, Quaternion::CreateRotation(x, y, z));
        }
        /** @brief Fills a Matrix4x4 with a rotation matrix from Euler angles. @param res Output matrix. @param x Pitch. @param y Yaw. @param z Roll. */
        static void GetRotation(Matrix4x4& res, double x, double y, double z) { Transform3D::GetRotation(res.Data(), x, y, z); }
        /** @brief Fills a raw array with a rotation matrix from a Vector3 of Euler angles. @param res Output array. @param amount Euler angles vector. */
        static void GetRotation(std::array<double, 16>& res, const Vector3& amount) { Transform3D::GetRotation(res, amount.x(), amount.y(), amount.z()); }
        /** @brief Fills a Matrix4x4 with a rotation matrix from a Vector3 of Euler angles. @param res Output matrix. @param amount Euler angles vector. */
        static void GetRotation(Matrix4x4& res, const Vector3& amount) { Transform3D::GetRotation(res.Data(), amount.x(), amount.y(), amount.z()); }
        /**
         * @brief Returns a 4×4 rotation matrix from Euler angles.
         * @param x Pitch (radians). @param y Yaw (radians). @param z Roll (radians).
         * @return Constructed Matrix4x4.
         */
        static Matrix4x4 GetRotation(double x, double y, double z) {
            std::array<double, 16> _res; Transform3D::GetRotation(_res, x, y, z); return _res;
        }
        /**
         * @brief Returns a 4×4 rotation matrix from a Vector3 of Euler angles.
         * @param amount Euler angles vector.
         * @return Constructed Matrix4x4.
         */
        static Matrix4x4 GetRotation(const Vector3& amount) {
            std::array<double, 16> _res; Transform3D::GetRotation(_res, amount); return _res;
        }
        /**
         * @brief Returns a 4×4 rotation matrix from a Quaternion.
         * @param rotation Source quaternion.
         * @return Constructed Matrix4x4.
         */
        static Matrix4x4 GetRotation(const Quaternion& rotation) {
            std::array<double, 16> _res; Transform3D::GetRotation(_res, rotation); return _res;
        }

        /**
         * @brief Fills a raw 16-element array with a combined 3D TRS matrix (T × R × S).
         * @param res         Output array (row-major 4×4).
         * @param translation Translation vector.
         * @param rotation    Euler rotation angles (radians).
         * @param scale       Scale vector.
         */
        static void GetTransformation(std::array<double, 16>& res, const Vector3& translation, const Vector3& rotation, const Vector3& scale);
        /** @brief Fills a Matrix4x4 with a combined 3D TRS matrix. @param res Output matrix. @param translation Translation. @param rotation Euler angles. @param scale Scale. */
        static void GetTransformation(Matrix4x4& res, const Vector3& translation, const Vector3& rotation, const Vector3& scale) {
            Transform3D::GetTransformation(res.Data(), translation, rotation, scale);
        }
        /**
         * @brief Returns a 4×4 combined 3D TRS matrix.
         * @param translation Translation vector.
         * @param rotation    Euler rotation angles (radians).
         * @param scale       Scale vector.
         * @return Constructed Matrix4x4.
         */
        static Matrix4x4 GetTransformation(const Vector3& translation, const Vector3& rotation, const Vector3& scale) {
            std::array<double, 16> _tmp;
            Transform3D::GetTransformation(_tmp, translation, rotation, scale);
            return Matrix4x4(_tmp);
        }

        /**
         * @brief Fills a raw 16-element array with an off-axis frustum projection matrix.
         * @param res    Output array (row-major 4×4).
         * @param left   Left clipping plane.
         * @param right  Right clipping plane.
         * @param top    Top clipping plane.
         * @param bottom Bottom clipping plane.
         * @param near   Near clipping distance.
         * @param far    Far clipping distance.
         */
        static void GetFrustumProjection(std::array<double, 16>& res, double left, double right, double top, double bottom, double near, double far);
        /** @brief Fills a Matrix4x4 with a frustum projection matrix. @param res Output matrix. @param left Left. @param right Right. @param top Top. @param bottom Bottom. @param near Near. @param far Far. */
        static void GetFrustumProjection(Matrix4x4& res, double left, double right, double top, double bottom, double near, double far) {
            Transform3D::GetFrustumProjection(res.Data(), left, right, top, bottom, near, far);
        }
        /**
         * @brief Returns a 4×4 frustum projection matrix.
         * @param left Left. @param right Right. @param top Top. @param bottom Bottom. @param near Near. @param far Far.
         * @return Constructed Matrix4x4.
         */
        static Matrix4x4 GetFrustumProjection(double left, double right, double top, double bottom, double near, double far) {
            std::array<double, 16> _tmp;
            Transform3D::GetFrustumProjection(_tmp, left, right, top, bottom, near, far);
            return Matrix4x4(_tmp);
        }

        /**
         * @brief Fills a raw 16-element array with a symmetric perspective projection matrix.
         * @param res    Output array (row-major 4×4).
         * @param fovY   Vertical field of view in radians.
         * @param aspect Viewport aspect ratio (width / height).
         * @param near   Near clipping distance.
         * @param far    Far clipping distance.
         */
        static void GetPerspectiveProjection(std::array<double, 16>& res, double fovY, double aspect, double near, double far);
        /** @brief Fills a Matrix4x4 with a perspective projection matrix. @param res Output matrix. @param fovY Vertical FOV (radians). @param aspect Aspect ratio. @param near Near plane. @param far Far plane. */
        static void GetPerspectiveProjection(Matrix4x4& res, double fovY, double aspect, double near, double far) {
            Transform3D::GetPerspectiveProjection(res.Data(), fovY, aspect, near, far);
        }
        /**
         * @brief Returns a 4×4 perspective projection matrix.
         * @param fovY   Vertical FOV in radians.
         * @param aspect Aspect ratio (width / height).
         * @param near   Near clipping distance.
         * @param far    Far clipping distance.
         * @return Constructed Matrix4x4.
         */
        static Matrix4x4 GetPerspectiveProjection(double fovY, double aspect, double near, double far) {
            std::array<double, 16> _tmp;
            Transform3D::GetPerspectiveProjection(_tmp, fovY, aspect, near, far);
            return Matrix4x4(_tmp);
        }

        /**
         * @brief Fills a raw 16-element array with an orthographic projection matrix.
         * @param res    Output array (row-major 4×4).
         * @param left   Left clipping plane.
         * @param right  Right clipping plane.
         * @param top    Top clipping plane.
         * @param bottom Bottom clipping plane.
         * @param near   Near clipping distance.
         * @param far    Far clipping distance.
         */
        static void GetOrthographicProjection(std::array<double, 16>& res, double left, double right, double top, double bottom, double near, double far);
        /** @brief Fills a Matrix4x4 with an orthographic projection matrix. @param res Output matrix. @param left Left. @param right Right. @param top Top. @param bottom Bottom. @param near Near. @param far Far. */
        static void GetOrthographicProjection(Matrix4x4& res, double left, double right, double top, double bottom, double near, double far) {
            Transform3D::GetOrthographicProjection(res.Data(), left, right, top, bottom, near, far);
        }
        /**
         * @brief Returns a 4×4 orthographic projection matrix.
         * @param left Left. @param right Right. @param top Top. @param bottom Bottom. @param near Near. @param far Far.
         * @return Constructed Matrix4x4.
         */
        static Matrix4x4 GetOrthographicProjection(double left, double right, double top, double bottom, double near, double far) {
            std::array<double, 16> _tmp;
            Transform3D::GetOrthographicProjection(_tmp, left, right, top, bottom, near, far);
            return Matrix4x4(_tmp);
        }
    };
}