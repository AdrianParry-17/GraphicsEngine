#include "TestHarness.h"
#include "TestSupport.h"

TEST_CASE(World_2DRenderGeometrySkipsInvalidIndices) {
    Tests::CountingGraphics2D sink;

    std::vector<Engine::Vertex2D<double>> v = {
        {0, 0, 1}, {1, 0, 2}, {0, 1, 3}, {1, 1, 4}
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
    Engine::Projected2DGraphics3D<double, double> projector(
        sink2d,
        [](const Engine::Vertex3D<double>& in, Engine::Vertex2D<double>& out) {
            out.x = in.x + 10;
            out.y = in.y - 5;
            out.color = in.color + 1;
        }
    );

    projector.RenderTriangle({1, 2, 3, 4}, {2, 3, 4, 5}, {3, 4, 5, 6});
    EXPECT_EQ(1, sink2d.triangles);
    EXPECT_NEAR(11.0, sink2d.lastA.x, 1e-12);
    EXPECT_NEAR(-3.0, sink2d.lastA.y, 1e-12);
    EXPECT_NEAR(5.0, sink2d.lastA.color, 1e-12);

    Tests::CountingGraphics4D sink4d;
    Engine::Embed4DGraphics3D<double, double> embedder(
        sink4d,
        [](const Engine::Vertex3D<double>& in, Engine::Vertex4D<double>& out) {
            out.x = in.x;
            out.y = in.y;
            out.z = in.z;
            out.w = 1.0;
            out.color = in.color;
        }
    );

    embedder.RenderTriangle({1,2,3,7}, {0,0,1,8}, {-1,-2,-3,9});
    EXPECT_EQ(1, sink4d.triangles);
    EXPECT_NEAR(1.0, sink4d.lastA.w, 1e-12);
    EXPECT_NEAR(7.0, sink4d.lastA.color, 1e-12);
}

TEST_CASE(World_3DAnd4DPlaneClippingRejectsOutsideTriangle) {
    Engine::Interpolator<double> interp;

    Tests::CountingGraphics3D sink3d;
    Engine::PlaneClippedGraphics3D<double> clip3d(sink3d, interp, {Engine::Plane3D(1, 0, 0, 0)}); // x >= 0
    clip3d.RenderTriangle({-2, 0, 0, 1}, {-1, 1, 0, 1}, {-1, -1, 0, 1});
    EXPECT_EQ(0, sink3d.triangles);

    Tests::CountingGraphics4D sink4d;
    Engine::PlaneClippedGraphics4D<double> clip4d(sink4d, interp, {Engine::Plane4D(1, 0, 0, 0, 0)}); // x >= 0
    clip4d.RenderTriangle({-2, 0, 0, 1, 1}, {-1, 1, 0, 1, 1}, {-1, -1, 0, 1, 1});
    EXPECT_EQ(0, sink4d.triangles);
}
