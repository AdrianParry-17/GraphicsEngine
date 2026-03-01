#pragma once

#include "Engine/Engine.h"

#include <algorithm>
#include <vector>

namespace Tests {
    class BufferContext : public Engine::IDrawingContext<double> {
    private:
        int width;
        int height;
        std::vector<double> pixels;

    public:
        BufferContext(int w, int h)
            : width(w), height(h), pixels(w * h, 0.0) {}

        bool SetPixel(int x, int y, const double& color) override {
            if (x < 0 || x >= width || y < 0 || y >= height)
                return false;
            pixels[y * width + x] = color;
            return true;
        }

        bool GetPixel(int x, int y, double& res) override {
            if (x < 0 || x >= width || y < 0 || y >= height)
                return false;
            res = pixels[y * width + x];
            return true;
        }

        void Clear(double v = 0.0) { std::fill(pixels.begin(), pixels.end(), v); }

        int CountNonZero() const {
            int count = 0;
            for (double p : pixels)
                if (p != 0.0)
                    ++count;
            return count;
        }
    };

    struct CountingGraphics2D : public Engine::IGraphics2D<double> {
        int triangles = 0;
        Engine::Vertex2D<double> lastA;
        Engine::Vertex2D<double> lastB;
        Engine::Vertex2D<double> lastC;

        void RenderTriangle(const Engine::Vertex2D<double>& a, const Engine::Vertex2D<double>& b, const Engine::Vertex2D<double>& c) override {
            ++triangles;
            lastA = a;
            lastB = b;
            lastC = c;
        }
    };

    struct CountingGraphics3D : public Engine::IGraphics3D<double> {
        int triangles = 0;
        Engine::Vertex3D<double> lastA;
        Engine::Vertex3D<double> lastB;
        Engine::Vertex3D<double> lastC;

        void RenderTriangle(const Engine::Vertex3D<double>& a, const Engine::Vertex3D<double>& b, const Engine::Vertex3D<double>& c) override {
            ++triangles;
            lastA = a;
            lastB = b;
            lastC = c;
        }
    };

    struct CountingGraphics4D : public Engine::IGraphics4D<double> {
        int triangles = 0;
        Engine::Vertex4D<double> lastA;
        Engine::Vertex4D<double> lastB;
        Engine::Vertex4D<double> lastC;

        void RenderTriangle(const Engine::Vertex4D<double>& a, const Engine::Vertex4D<double>& b, const Engine::Vertex4D<double>& c) override {
            ++triangles;
            lastA = a;
            lastB = b;
            lastC = c;
        }
    };
}
