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

    struct CountingGraphics2D : public Engine::IWorldGraphics2D<double> {
        int triangles = 0;
        Engine::WorldVertex2D<double> lastA;
        Engine::WorldVertex2D<double> lastB;
        Engine::WorldVertex2D<double> lastC;

        void RenderTriangle(const Engine::WorldVertex2D<double>& a, const Engine::WorldVertex2D<double>& b, const Engine::WorldVertex2D<double>& c) override {
            ++triangles;
            lastA = a;
            lastB = b;
            lastC = c;
        }
    };

    struct CountingGraphics3D : public Engine::IWorldGraphics3D<double> {
        int triangles = 0;
        Engine::WorldVertex3D<double> lastA;
        Engine::WorldVertex3D<double> lastB;
        Engine::WorldVertex3D<double> lastC;

        void RenderTriangle(const Engine::WorldVertex3D<double>& a, const Engine::WorldVertex3D<double>& b, const Engine::WorldVertex3D<double>& c) override {
            ++triangles;
            lastA = a;
            lastB = b;
            lastC = c;
        }
    };

    struct CountingGraphics4D : public Engine::IWorldGraphics4D<double> {
        int triangles = 0;
        Engine::WorldVertex4D<double> lastA;
        Engine::WorldVertex4D<double> lastB;
        Engine::WorldVertex4D<double> lastC;

        void RenderTriangle(const Engine::WorldVertex4D<double>& a, const Engine::WorldVertex4D<double>& b, const Engine::WorldVertex4D<double>& c) override {
            ++triangles;
            lastA = a;
            lastB = b;
            lastC = c;
        }
    };
}
