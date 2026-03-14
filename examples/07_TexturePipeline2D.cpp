#include "Engine/Engine.h"
#include "common/TerminalBufferContext.h"
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <thread>

int main() {
    Engine::ScalableInterpolator<double> color_interpolator;
    Engine::TexturedColorScalableInterpolator<double> tex_color_interpolator(color_interpolator);

    // - Texture Initialize
    //  + A simple 2x2 checker board pattern texture.
    Engine::FunctionTexture<double> func_tex([](int x, int y, double& res) {
        if (x < 0 || x > 1 || y < 0 || y > 1) return false;
        res = ((x + y) % 2 == 0) ? 9 : 3;
        return true;
    });
    //  + Making the texture repeated
    Engine::RepeatedTexture<double> rep_tex(func_tex, Engine::Rectangle(0, 0, 2, 2));
    //  + Adapting to 2D texture mapping, using Nearest Neighbor mapping algorithm
    Engine::NativeTexture2D<double> native_tex2d(rep_tex, color_interpolator, Engine::NativeTextureAdaptMethod::NearestNeighbor);
    //  + UV mapping:
    //    - Input (0, 0) -> (1, 1)
    //    - Output (0, 0) -> (2, 2)
    Engine::ViewportTransformTexture2D<double> viewport_tex2d(
        native_tex2d, Engine::RangeMapper(0, 1, 0, 2), Engine::RangeMapper(0, 1, 0, 2)
    );

    // - Graphics Initialize
    TerminalBufferContext context(120, 30);
    Engine::ContextGraphics<double> context_g(context);
    //   + Add texture sampling layer
    Engine::TextureSamplingGraphics<double> tex_sampling_g(context_g);

    Engine::Graphics2DPipeline<Engine::TexturedColor<double>> pipeline2d(
        tex_sampling_g, tex_color_interpolator,
        Engine::Rectangle(0, 0, 120, 30)
    );

    const double _pi = std::acos(-1);
    pipeline2d.SetWorldCamera(2, 120.0 / 30.0);

    for (int i = -180; i <= 180; ++i) {
        context.Clear();

        // 2. Draw animated Square 1 (with texture)
        tex_sampling_g.SetTargetTexture(&viewport_tex2d);

        pipeline2d.PushMatrix();
        pipeline2d.Transform2D(
            Engine::Vector2(0.5, 0),         // Translate
            (i * 4) * _pi / 180,             // Rotate
            Engine::Vector2(1, 1)            // Scale
        );
        pipeline2d.RenderGeometry(
            {
                Engine::WorldVertex2D<Engine::TexturedColor<double>>({-0.5, 0.5}, {2, 0.0, 1.0}),  // Top Left (color 2)
                Engine::WorldVertex2D<Engine::TexturedColor<double>>({0.5, 0.5}, {4, 1.0, 1.0}),   // Top Right (color 4)
                Engine::WorldVertex2D<Engine::TexturedColor<double>>({-0.5, -0.5}, {6, 0.0, 0.0}), // Bottom Left (color 6)
                Engine::WorldVertex2D<Engine::TexturedColor<double>>({0.5, -0.5}, {8, 1.0, 0.0})   // Bottom Right (color 8)
            },
            {0, 1, 2, 1, 2, 3} // Triangles indices
        );
        pipeline2d.PopMatrix();

        // 3. Draw animated Square 2 (no texture)
        tex_sampling_g.SetTargetTexture(nullptr);

        pipeline2d.PushMatrix();
        pipeline2d.Transform2D(
            Engine::Vector2(3.75, 0),        // Translate
            (-i * 4) * _pi / 180,            // Rotate opposite direction
            Engine::Vector2(1, 1)            // Scale
        );
        pipeline2d.RenderGeometry(
            {
                Engine::WorldVertex2D<Engine::TexturedColor<double>>({-0.5, 0.5}, {2, 0.0, 1.0}),  // Top Left (color 2)
                Engine::WorldVertex2D<Engine::TexturedColor<double>>({0.5, 0.5}, {4, 1.0, 1.0}),   // Top Right (color 4)
                Engine::WorldVertex2D<Engine::TexturedColor<double>>({-0.5, -0.5}, {6, 0.0, 0.0}), // Bottom Left (color 6)
                Engine::WorldVertex2D<Engine::TexturedColor<double>>({0.5, -0.5}, {8, 1.0, 0.0})   // Bottom Right (color 8)
            },
            {0, 1, 2, 1, 2, 3});
        pipeline2d.PopMatrix();

        // Output rasterized buffer to console
        std::system("clear");
        context.Write();

        // Introduce small frame delay for rendering playback
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }

    context.Report();
    return 0;
}
