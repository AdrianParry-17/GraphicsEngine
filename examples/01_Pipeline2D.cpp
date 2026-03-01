#include "Engine/Engine.h"
#include "Engine/Engine_2D.h"
#include "Engine/Engine_Geometry.h"
#include "Engine/Engine_Graphics.h"
#include "Engine/Engine_Graphics2DPipeline.h"
#include "Engine/Engine_Interpolation.h"
#include "common/TerminalBufferContext.h"
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <thread>

int main() {
    // Initialize standard terminal buffer (width = 120, height = 30)
    TerminalBufferContext context(120, 30);
    Engine::Interpolator<double> color_interpolator;

    // Connect raw pixel buffer to a graphics context
    Engine::ContextGraphics<double> context_g(context);

    // Setup 2D graphics pipeline mapping over the entire buffer view
    Engine::Graphics2DPipeline<double> pipeline2d(
        context_g, color_interpolator,
        Engine::Rectangle(0, 0, 120, 30));

    // Define PI for rotation calculations
    const double _pi = std::acos(-1);

    // 1. Setup global camera viewport boundary
    // Aspect ratio 120/30 = 4, so scale X logically to fit Y. We set horizontal span mapping (-4..4).
    pipeline2d.SetWorldCamera(2, 120.0 / 30.0);

    for (int i = -180; i <= 180; ++i) {
        context.Clear(); // Blank frame

        // 2. Draw animated Square 1
        pipeline2d.PushMatrix();
        pipeline2d.Transform2D(
            Engine::Vector2(0.5, 0),         // Translate
            (i * 4) * _pi / 180,           // Rotate
            Engine::Vector2(1, 1)            // Scale
        );
        pipeline2d.RenderGeometry(
            {
                Engine::Vertex2D<double>(-0.5, 0.5, 2),  // Top Left (color 2)
                Engine::Vertex2D<double>(0.5, 0.5, 4),   // Top Right (color 4)
                Engine::Vertex2D<double>(-0.5, -0.5, 6), // Bottom Left (color 6)
                Engine::Vertex2D<double>(0.5, -0.5, 8)   // Bottom Right (color 8)
            },
            {0, 1, 2, 1, 2, 3} // Triangles indices
        );
        pipeline2d.PopMatrix();

        // 3. Draw animated Square 2
        pipeline2d.PushMatrix();
        pipeline2d.Transform2D(
            Engine::Vector2(3.75, 0),        // Translate
            (-i * 4) * _pi / 180,          // Rotate opposite direction
            Engine::Vector2(1, 1)            // Scale
        );
        pipeline2d.RenderGeometry(
            {
                Engine::Vertex2D<double>(-0.5, 0.5, 2),
                Engine::Vertex2D<double>(0.5, 0.5, 4),
                Engine::Vertex2D<double>(-0.5, -0.5, 6),
                Engine::Vertex2D<double>(0.5, -0.5, 8)
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
