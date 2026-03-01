#include "TestHarness.h"
#include "TestSupport.h"

#include <stdexcept>

TEST_CASE(Pipeline_2DSmokeRenderTriangleToContext) {
    Tests::BufferContext ctx(20, 10);
    Engine::ContextGraphics<double> graphics(ctx);
    Engine::Interpolator<double> interp;

    Engine::Graphics2DPipeline<double> pipe(graphics, interp, Engine::Rectangle(0, 0, 20, 10));
    pipe.SetWorldCamera(2.0, 2.0);
    pipe.LoadIdentity();

    pipe.RenderTriangle(
        Engine::Vertex2D<double>(-0.5, -0.5, 2.0),
        Engine::Vertex2D<double>(0.5, -0.5, 2.0),
        Engine::Vertex2D<double>(0.0, 0.5, 2.0)
    );

    EXPECT_TRUE(ctx.CountNonZero() > 0);
}

TEST_CASE(Pipeline_3DDepthBufferAndViewportControls) {
    Tests::BufferContext ctx(20, 10);
    Engine::ContextGraphics<double> graphics(ctx);
    Engine::Interpolator<double> interp;

    Engine::Graphics3DPipeline<double> pipe(graphics, interp, Engine::Rectangle(0, 0, 20, 10));
    EXPECT_TRUE(!pipe.IsDepthBufferEnabled());

    pipe.EnableDepthBuffer();
    EXPECT_TRUE(pipe.IsDepthBufferEnabled());
    pipe.ClearDepthBuffer();

    const Engine::Vertex3D<double> a(-0.7, -0.7, 0.0, 2.0);
    const Engine::Vertex3D<double> b(0.7, -0.7, 0.0, 2.0);
    const Engine::Vertex3D<double> c(0.0, 0.7, 0.0, 2.0);

    pipe.RenderTriangle(a, b, c); // near

    const Engine::Vertex3D<double> af(-0.7, -0.7, 0.5, 9.0);
    const Engine::Vertex3D<double> bf(0.7, -0.7, 0.5, 9.0);
    const Engine::Vertex3D<double> cf(0.0, 0.7, 0.5, 9.0);
    pipe.RenderTriangle(af, bf, cf); // farther, should fail depth where overlapping

    double center = 0.0;
    EXPECT_TRUE(ctx.GetPixel(10, 5, center));
    EXPECT_NEAR(2.0, center, 1e-9);

    pipe.DisableDepthBuffer();
    EXPECT_TRUE(!pipe.IsDepthBufferEnabled());

    bool thrown = false;
    try {
        pipe.SetViewport(Engine::Rectangle(0, 0, 0, 10));
    } catch (const std::invalid_argument&) {
        thrown = true;
    }
    EXPECT_TRUE(thrown);
}

TEST_CASE(Pipeline_3DRenderGeometrySharedVertices) {
    Tests::BufferContext ctx(24, 12);
    Engine::ContextGraphics<double> graphics(ctx);
    Engine::Interpolator<double> interp;

    Engine::Graphics3DPipeline<double> pipe(graphics, interp, Engine::Rectangle(0, 0, 24, 12));

    std::vector<Engine::Vertex3D<double>> vertices = {
        {-0.7, -0.7, 0.0, 3.0},
        {0.7, -0.7, 0.0, 5.0},
        {0.0, 0.7, 0.0, 7.0}
    };
    std::vector<int> indices = {0, 1, 2};

    pipe.RenderGeometry(vertices, indices);
    EXPECT_TRUE(ctx.CountNonZero() > 0);
}
