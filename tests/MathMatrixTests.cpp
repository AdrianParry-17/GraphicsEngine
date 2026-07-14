#include "TestHarness.h"
#include "TestSupport.h"

#include <type_traits>

TEST_CASE(Math_QuaternionIdentityRotation) {
    Engine::Quaternion q = Engine::Quaternion::CreateRotation(0.0, 0.0, 0.0);
    EXPECT_NEAR(1.0, q.a, 1e-12);
    EXPECT_NEAR(0.0, q.b, 1e-12);
    EXPECT_NEAR(0.0, q.c, 1e-12);
    EXPECT_NEAR(0.0, q.d, 1e-12);
}

TEST_CASE(Math_VectorCrossDimensionComparisonTreatsMissingComponentsAsZero) {
    static_assert(std::is_same<bool, decltype(Engine::Vector2() == Engine::Vector3())>::value,
        "Cross-dimension vector comparison must return bool");
    static_assert(std::is_same<bool, decltype(Engine::Vector2().IsIdentical(Engine::Vector3()))>::value,
        "Cross-dimension vector identity check must return bool");

    const Engine::Vector2 xy(1.0, 2.0);
    const Engine::Vector3 xyz_zero(1.0, 2.0, 0.0);
    const Engine::Vector3 xyz_nonzero(1.0, 2.0, 3.0);
    const Engine::Vector3 xyz_near_zero(1.0 + 1e-10, 2.0, 1e-10);

    EXPECT_TRUE(xy == xyz_zero);
    EXPECT_TRUE(xyz_zero == xy);
    EXPECT_TRUE(xy != xyz_nonzero);
    EXPECT_TRUE(xyz_nonzero != xy);

    EXPECT_TRUE(xy.IsIdentical(xyz_near_zero));
    EXPECT_TRUE(!xy.IsIdentical(xyz_nonzero));
    EXPECT_TRUE(!xyz_nonzero.IsIdentical(xy));
}

TEST_CASE(Math_Matrix4x4Transformation3DComposition) {
    std::array<double, 16> data;
    Engine::Transform3D::GetTransformation(
        data,
        Engine::Vector3(2, 3, 4),
        Engine::Vector3(0, 0, 0),
        Engine::Vector3(2, 3, 4)
    );

    Engine::Matrix4x4 m(data);
    Engine::Vector4 out = m * Engine::Vector4(1, 1, 1, 1);

    EXPECT_NEAR(4.0, out.x(), 1e-9);
    EXPECT_NEAR(6.0, out.y(), 1e-9);
    EXPECT_NEAR(8.0, out.z(), 1e-9);
    EXPECT_NEAR(1.0, out.w(), 1e-9);
}

TEST_CASE(Math_Matrix4x4PerspectiveInvalidReturnsIdentity) {
    std::array<double, 16> data;
    Engine::Transform3D::GetPerspectiveProjection(data, 1.0, 1.0, 2.0, 1.0); // near >= far

    EXPECT_NEAR(1.0, data[0], 1e-12);
    EXPECT_NEAR(1.0, data[5], 1e-12);
    EXPECT_NEAR(1.0, data[10], 1e-12);
    EXPECT_NEAR(1.0, data[15], 1e-12);
    for (int i : {1,2,3,4,6,7,8,9,11,12,13,14})
        EXPECT_NEAR(0.0, data[i], 1e-12);
}

TEST_CASE(Math_Matrix3x3ViewportTransformMapping) {
    std::array<double, 9> data;
    Engine::Transform2D::GetViewportTransform(
        data,
        -1.0, 1.0, 1.0, -1.0,
        0.0, 10.0, 0.0, 10.0
    );

    Engine::Matrix3x3 m(data);
    Engine::Vector3 p = m * Engine::Vector3(0, 0, 1);
    EXPECT_NEAR(5.0, p.x(), 1e-9);
    EXPECT_NEAR(5.0, p.y(), 1e-9);
    EXPECT_NEAR(1.0, p.z(), 1e-9);
}
