#pragma once

#include "Engine/Engine_Graphics.h"
#include <cmath>
#include <iostream>
#include <vector>

class TerminalBufferContext : public Engine::IDrawingContext<double> {
private:
    std::vector<std::vector<double>> _buffer;
    int _w, _h;
    int out_bound = 0;

public:
    TerminalBufferContext(int w, int h)
        : _w(w), _h(h), _buffer(h, std::vector<double>(w, 0)) {}

    bool GetPixel(int x, int y, double &color) override {
        if (x < 0 || x >= _w || y < 0 || y >= _h) {
            ++out_bound;
            return false;
        }
        color = _buffer[y][x];
        return true;
    }

    bool SetPixel(int x, int y, const double &color) override {
        if (x < 0 || x >= _w || y < 0 || y >= _h) {
            ++out_bound;
            return false;
        }
        _buffer[y][x] = color;
        return true;
    }

    void Write() {
        const char ascii_brightness[] = " .:-=+*#%@";
        std::cout << '+' << std::string(_w, '-') << "+\n";
        for (int y = 0; y < _h; ++y) {
            std::cout << '|';
            for (int x = 0; x < _w; ++x) {
                int val = std::max(0, std::min(9, (int)std::round(_buffer[y][x])));
                std::cout << ascii_brightness[val];
            }
            std::cout << "|\n";
        }
        std::cout << '+' << std::string(_w, '-') << '+' << std::endl;
    }
    void Report() {
        std::cout << "Context Report:\n";
        std::cout << "+ Out of bound requested pixel: " << out_bound << '\n';
        std::cout << std::flush;
    }

    void Clear() { 
        _buffer.assign(_h, std::vector<double>(_w, 0)); 
    }
};
