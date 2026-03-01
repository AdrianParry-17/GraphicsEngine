#include "TestHarness.h"
#include "TestSupport.h"

TEST_CASE(Math_QuaternionIdentityRotation) {
    Engine::Quaternion q = Engine::Quaternion::CreateRotation(0.0, 0.0, 0.0);
    EXPECT_NEAR(1.0, q.a, 1e-12);
    EXPECT_NEAR(0.0, q.b, 1e-12);
    EXPECT_NEAR(0.0, q.c, 1e-12);
    EXPECT_NEAR(0.0, q.d, 1e-12);
}

TEST_CASE(Math_Matrix4x4Transformation3DComposition) {
    std::array<double, 16> data;
    Engine::Matrix4x4::GetTransformation3D(
        data,
        Engine::Vector3(2, 3, 4),
        Engine::Vector3(0, 0, 0),
        Engine::Vector3(2, 3, 4)
    );

    Engine::Matrix4x4 m(data);
    Engine::Vector4 out = m * Engine::Vector4(1, 1, 1, 1);

    EXPECT_NEAR(4.0, out.x, 1e-9);
    EXPECT_NEAR(6.0, out.y, 1e-9);
    EXPECT_NEAR(8.0, out.z, 1e-9);
    EXPECT_NEAR(1.0, out.w, 1e-9);
}

TEST_CASE(Math_Matrix4x4PerspectiveInvalidReturnsIdentity) {
    std::array<double, 16> data;
    Engine::Matrix4x4::GetPerspectiveProjection(data, 1.0, 1.0, 2.0, 1.0); // near >= far

    EXPECT_NEAR(1.0, data[0], 1e-12);
    EXPECT_NEAR(1.0, data[5], 1e-12);
    EXPECT_NEAR(1.0, data[10], 1e-12);
    EXPECT_NEAR(1.0, data[15], 1e-12);
    for (int i : {1,2,3,4,6,7,8,9,11,12,13,14})
        EXPECT_NEAR(0.0, data[i], 1e-12);
}

TEST_CASE(Math_Matrix3x3ViewportTransformMapping) {
    std::array<double, 9> data;
    Engine::Matrix3x3::GetViewportTransform2D(
        data,
        -1.0, 1.0, 1.0, -1.0,
        0.0, 10.0, 0.0, 10.0
    );

    Engine::Matrix3x3 m(data);
    Engine::Vector3 p = m * Engine::Vector3(0, 0, 1);
    EXPECT_NEAR(5.0, p.x, 1e-9);
    EXPECT_NEAR(5.0, p.y, 1e-9);
    EXPECT_NEAR(1.0, p.z, 1e-9);
}
