#include "TestHarness.h"
#include "TestSupport.h"

TEST_CASE(World_2DRenderGeometrySkipsInvalidIndices) {
    Tests::CountingGraphics2D sink;

    std::vector<Engine::WorldVertex2D<double>> v = {
        Engine::WorldVertex2D<double>({0, 0}, 1),
        Engine::WorldVertex2D<double>({1, 0}, 2),
        Engine::WorldVertex2D<double>({0, 1}, 3),
        Engine::WorldVertex2D<double>({1, 1}, 4)
    };
    std::vector<int> idx = {
        0, 1, 2,
        0, 2, 3,
        -1, 0, 1,
        0, 1, 100
    };

    sink.RenderGeometry(v, idx);
    EXPECT_EQ(2, sink.triangles);
}

TEST_CASE(World_3DProjectorAnd4DEmbedderAdapters) {
    Tests::CountingGraphics2D sink2d;
    Engine::VertexDimensionConvertWorldGraphics<3, 2, double> projector(
        sink2d,
        [](const Engine::WorldVertex3D<double>& in, Engine::WorldVertex2D<double>& out) {
            out.position.x() = in.position.x() + 10;
            out.position.y() = in.position.y() - 5;
            out.color = in.color + 1;
        }
    );

    projector.RenderTriangle(
        Engine::WorldVertex3D<double>({1, 2, 3}, 4),
        Engine::WorldVertex3D<double>({2, 3, 4}, 5),
        Engine::WorldVertex3D<double>({3, 4, 5}, 6)
    );
    EXPECT_EQ(1, sink2d.triangles);
    EXPECT_NEAR(11.0, sink2d.lastA.position.x(), 1e-12);
    EXPECT_NEAR(-3.0, sink2d.lastA.position.y(), 1e-12);
    EXPECT_NEAR(5.0, sink2d.lastA.color, 1e-12);

    Tests::CountingGraphics4D sink4d;
    Engine::VertexDimensionConvertWorldGraphics<3, 4, double> embedder(
        sink4d,
        [](const Engine::WorldVertex3D<double>& in, Engine::WorldVertex4D<double>& out) {
            out.position.x() = in.position.x();
            out.position.y() = in.position.y();
            out.position.z() = in.position.z();
            out.position.w() = 1.0;
            out.color = in.color;
        }
    );

    embedder.RenderTriangle(
        Engine::WorldVertex3D<double>({ 1,  2,  3}, 7),
        Engine::WorldVertex3D<double>({ 0,  0,  1}, 8),
        Engine::WorldVertex3D<double>({-1, -2, -3}, 9)
    );
    EXPECT_EQ(1, sink4d.triangles);
    EXPECT_NEAR(1.0, sink4d.lastA.position.w(), 1e-12);
    EXPECT_NEAR(7.0, sink4d.lastA.color, 1e-12);
}

TEST_CASE(World_3DAnd4DPlaneClippingRejectsOutsideTriangle) {
    Engine::Interpolator<double> interp;

    Tests::CountingGraphics3D sink3d;
    Engine::PlaneClipWorldGraphics3D<double> clip3d(sink3d, interp, {Engine::WorldPlane3D(1, 0, 0, 0)}); // x >= 0
    clip3d.RenderTriangle(
        Engine::WorldVertex3D<double>({-2,  0, 0}, 1),
        Engine::WorldVertex3D<double>({-1,  1, 0}, 1),
        Engine::WorldVertex3D<double>({-1, -1, 0}, 1)
    );
    EXPECT_EQ(0, sink3d.triangles);

    Tests::CountingGraphics4D sink4d;
    Engine::PlaneClipWorldGraphics4D<double> clip4d(sink4d, interp, {Engine::WorldPlane4D(1, 0, 0, 0, 0)}); // x >= 0
    clip4d.RenderTriangle(
        Engine::WorldVertex4D<double>({-2,  0, 0, 1}, 1),
        Engine::WorldVertex4D<double>({-1,  1, 0, 1}, 1),
        Engine::WorldVertex4D<double>({-1, -1, 0, 1}, 1)
    );
    EXPECT_EQ(0, sink4d.triangles);
}
