#include "TestHarness.h"
#include "TestSupport.h"

TEST_CASE(Graphics_ContextViewportClipAndPrimitive) {
    Tests::BufferContext ctx(8, 8);
    Engine::ContextGraphics<double> base(ctx);

    EXPECT_TRUE(base.DrawPoint(1, 2, 7.0));
    double v = 0.0;
    EXPECT_TRUE(ctx.GetPixel(1, 2, v));
    EXPECT_EQ(7.0, v);

    Engine::ViewportGraphics<double> viewport(base, Engine::Rectangle(2, 3, 4, 4));
    EXPECT_TRUE(viewport.DrawPoint(0, 0, 3.0));
    EXPECT_TRUE(ctx.GetPixel(2, 3, v));
    EXPECT_EQ(3.0, v);
    EXPECT_TRUE(!viewport.DrawPoint(4, 0, 1.0));

    Engine::ClippedGraphics<double> clipped(base, Engine::Rectangle(1, 1, 3, 3));
    EXPECT_TRUE(clipped.DrawPoint(2, 2, 5.0));
    EXPECT_TRUE(!clipped.DrawPoint(7, 7, 5.0));

    Engine::PrimitiveGraphics<double> prim(base);
    prim.DrawHorizontalLine(1, 6, 3, 9.0);
    EXPECT_TRUE(ctx.GetPixel(1, 6, v));
    EXPECT_EQ(9.0, v);
    EXPECT_TRUE(ctx.GetPixel(2, 6, v));
    EXPECT_EQ(9.0, v);
    EXPECT_TRUE(ctx.GetPixel(3, 6, v));
    EXPECT_EQ(9.0, v);
}

TEST_CASE(Graphics_RendererLineInterpolationDouble) {
    Tests::BufferContext ctx(10, 3);
    Engine::ContextGraphics<double> base(ctx);
    Engine::Interpolator<double> interp;
    Engine::RendererGraphics<double> renderer(base, interp);

    renderer.RenderLine(1, 1, 3, 1, 0.0, 10.0);

    double a = -1, b = -1, c = -1;
    EXPECT_TRUE(ctx.GetPixel(1, 1, a));
    EXPECT_TRUE(ctx.GetPixel(2, 1, b));
    EXPECT_TRUE(ctx.GetPixel(3, 1, c));

    EXPECT_NEAR(0.0, a, 1e-9);
    EXPECT_NEAR(5.0, b, 1e-9);
    EXPECT_NEAR(10.0, c, 1e-9);
}
