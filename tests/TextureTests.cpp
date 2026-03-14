#include "TestHarness.h"
#include "TestSupport.h"

TEST_CASE(Texture_BasicAdapters) {
    Tests::BufferContext ctx(4, 4);
    for (int y = 0; y < 4; ++y)
        for (int x = 0; x < 4; ++x)
            EXPECT_TRUE(ctx.SetPixel(x, y, y * 10.0 + x));

    Engine::ContextTexture<double> context_texture(ctx);

    double value = -1.0;
    EXPECT_TRUE(context_texture.GetColor(2, 3, value));
    EXPECT_EQ(32.0, value);

    Engine::ViewportTexture<double> viewport_texture(context_texture, Engine::Rectangle(1, 1, 2, 2));
    EXPECT_TRUE(viewport_texture.GetColor(0, 0, value));
    EXPECT_EQ(11.0, value);
    EXPECT_TRUE(viewport_texture.GetColor(1, 1, value));
    EXPECT_EQ(22.0, value);
    EXPECT_TRUE(!viewport_texture.GetColor(2, 0, value));

    Engine::ClippedTexture<double> clipped_texture(context_texture, Engine::Rectangle(1, 1, 2, 2));
    EXPECT_TRUE(clipped_texture.GetColor(1, 1, value));
    EXPECT_EQ(11.0, value);
    EXPECT_TRUE(!clipped_texture.GetColor(0, 0, value));

    Engine::RepeatedTexture<double> repeated_texture(context_texture, Engine::Rectangle(1, 1, 2, 2));
    EXPECT_TRUE(repeated_texture.GetColor(3, 3, value));
    EXPECT_EQ(11.0, value);
    EXPECT_TRUE(repeated_texture.GetColor(4, 2, value));
    EXPECT_EQ(22.0, value);
}

TEST_CASE(Texture_ConvertTextureConvertsPayloadType) {
    Engine::FunctionTexture<int> src_texture([](int x, int y, int& result) {
        if (x == 1 && y == 2) {
            result = 7;
            return true;
        }
        return false;
    });

    Engine::ConvertTexture<int, double> converted_texture(
        src_texture,
        [](const int& input, double& output) {
            output = input * 0.5;
        }
    );

    double value = 0.0;
    EXPECT_TRUE(converted_texture.GetColor(1, 2, value));
    EXPECT_NEAR(3.5, value, 1e-9);
    EXPECT_TRUE(!converted_texture.GetColor(0, 0, value));
}

TEST_CASE(Texture2D_NativeSamplingAndViewportMapping) {
    Engine::FunctionTexture<double> src_texture([](int x, int y, double& result) {
        if (x < 0 || x > 1 || y < 0 || y > 1)
            return false;
        result = y * 20.0 + x * 10.0;
        return true;
    });
    Engine::Interpolator<double> interpolator;

    Engine::NativeTexture2D<double> bilinear_texture(src_texture, interpolator, Engine::NativeTextureAdaptMethod::Bilinear);

    double value = 0.0;
    EXPECT_TRUE(bilinear_texture.GetColor(0.5, 0.5, value));
    EXPECT_NEAR(15.0, value, 1e-9);
    EXPECT_TRUE(bilinear_texture.GetColor(1.0, 0.25, value));
    EXPECT_NEAR(15.0, value, 1e-9);

    Engine::NativeTexture2D<double> nearest_texture(src_texture, interpolator, Engine::NativeTextureAdaptMethod::NearestNeighbor);
    EXPECT_TRUE(nearest_texture.GetColor(1.0, 1.0, value));
    EXPECT_NEAR(30.0, value, 1e-9);

    Engine::FunctionTexture2D<double> analytic_texture([](double x, double y, double& result) {
        result = x * 10.0 + y;
        return true;
    });
    Engine::ViewportTransformTexture2D<double> mapped_texture(
        analytic_texture,
        Engine::RangeMapper(0.0, 1.0, 2.0, 4.0),
        Engine::RangeMapper(0.0, 1.0, -1.0, 1.0)
    );

    EXPECT_TRUE(mapped_texture.GetColor(0.5, 0.25, value));
    EXPECT_NEAR(29.5, value, 1e-9);
}

TEST_CASE(Texture_TexturedInterpolatorsPreserveColorAndUV) {
    Engine::Interpolator<double> linear_color_interpolator;
    Engine::TexturedColorInterpolator<double> textured_interpolator(linear_color_interpolator);

    const Engine::TexturedColor<double> a(2.0, 0.0, 0.0);
    const Engine::TexturedColor<double> b(6.0, 1.0, 1.0);
    Engine::TexturedColor<double> result;

    textured_interpolator.Linear(a, b, 0.25, result);
    EXPECT_NEAR(3.0, result.color, 1e-9);
    EXPECT_NEAR(0.25, result.uv_x, 1e-9);
    EXPECT_NEAR(0.25, result.uv_y, 1e-9);

    Engine::ScalableInterpolator<double> scalable_color_interpolator;
    Engine::TexturedColorScalableInterpolator<double> scalable_textured_interpolator(scalable_color_interpolator);
    scalable_textured_interpolator.Scale(Engine::TexturedColor<double>(4.0, 0.25, 0.5), 2.0, result);
    EXPECT_NEAR(8.0, result.color, 1e-9);
    EXPECT_NEAR(0.5, result.uv_x, 1e-9);
    EXPECT_NEAR(1.0, result.uv_y, 1e-9);
}

TEST_CASE(Texture_TextureSamplingGraphicsUsesTextureAndFallback) {
    Tests::BufferContext ctx(4, 4);
    Engine::ContextGraphics<double> base_graphics(ctx);
    Engine::TextureSamplingGraphics<double> textured_graphics(base_graphics);

    double value = 0.0;
    EXPECT_TRUE(textured_graphics.DrawPoint(1, 1, Engine::TexturedColor<double>(5.0, 0.2, 0.4)));
    EXPECT_TRUE(ctx.GetPixel(1, 1, value));
    EXPECT_NEAR(5.0, value, 1e-9);

    Engine::FunctionTexture2D<double> texture([](double x, double y, double& result) {
        result = x * 10.0 + y;
        return true;
    });
    textured_graphics.SetTargetTexture(&texture);

    EXPECT_TRUE(textured_graphics.DrawPoint(2, 2, Engine::TexturedColor<double>(7.0, 2.5, 1.5)));
    EXPECT_TRUE(ctx.GetPixel(2, 2, value));
    EXPECT_NEAR(26.5, value, 1e-9);

    Engine::FunctionTexture2D<double> failing_texture([](double, double, double&) {
        return false;
    });
    textured_graphics.SetTargetTexture(&failing_texture);

    EXPECT_TRUE(textured_graphics.DrawPoint(3, 3, Engine::TexturedColor<double>(8.0, 9.0, 9.0)));
    EXPECT_TRUE(ctx.GetPixel(3, 3, value));
    EXPECT_NEAR(8.0, value, 1e-9);
}