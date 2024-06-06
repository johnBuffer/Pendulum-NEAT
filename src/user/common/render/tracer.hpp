#pragma once
#include "engine/engine.hpp"


struct Tracer
{
    sf::Color                color = sf::Color::White;
    std::vector<Vec2>        points;
    std::vector<SmoothFloat> width;
    sf::VertexArray          va_line;

    float width_start = 4.0f;
    float width_end   = 0.0f;
    float width_speed = 0.0f;
    Interpolation interpolation = Interpolation::EaseInOutQuint;

    explicit
    Tracer()
        : va_line{sf::PrimitiveType::TriangleStrip}
    {}

    void setColor(sf::Color c)
    {
        color = c;
    }

    void clear()
    {
        points.clear();
        width.clear();
    }

    void addPoint(Vec2 pt)
    {
        points.push_back(pt);
        auto& w = width.emplace_back();
        w.setValueInstant(width_start);
        w.setInterpolationFunction(interpolation);
        w.setSpeed(width_speed);
        w = width_end;
    }

    void render(pez::render::Context& context)
    {
        va_line.resize(2 * points.size() - 2);

        if (points.size() > 1) {
            Vec2 last = points[0];
            uint32_t i{0};
            for (auto const& pt: points) {
                if (i > 0) {
                    Vec2 const current = pt;
                    Vec2 const d = current - last;
                    Vec2 const n = MathVec2::normalize(MathVec2::normal(d));
                    float const w = width[i].get();
                    va_line[2 * (i - 1) + 0].position = current + w * n;
                    va_line[2 * (i - 1) + 1].position = current - w * n;
                    va_line[2 * (i - 1) + 0].color = color;
                    va_line[2 * (i - 1) + 1].color = color;
                    last = current;
                }
                ++i;
            }
            context.draw(va_line);
        }
    }
};
