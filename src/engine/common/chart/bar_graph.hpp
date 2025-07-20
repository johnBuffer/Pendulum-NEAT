#pragma once
#include <SFML/Graphics.hpp>

#include "engine/common/math.hpp"
#include "engine/common/racc.hpp"
#include "engine/common/quad_vertex_array.hpp"
#include "engine/render/render_context.hpp"

struct BarGraph
{
    // Data
    RAccBase<float> data;
    Vec2            extremes     = {};
    uint32_t        samples_count = 0;

    // Geometry
    pez::QuadVertexArray va_bar;
    pez::QuadVertexArray va_lines;
    sf::Transform transform;

    Vec2 size           = {};
    Vec2 position       = {};
    float space_x       = 2.0f;
    float header_height = 4.0f;

    // Color
    sf::Color accent_color;

    explicit
    BarGraph(sf::Vector2f const size_)
        : data(100)
        , size{size_}
        , accent_color{sf::Color::Red}
    {}

    void draw(pez::render::Context& context) const
    {
        context.drawDirect(va_bar.asVertexArray(), transform);
        context.drawDirect(va_lines.asVertexArray(), transform);
    }

    void addValue(float const value)
    {
        ++samples_count;
        data.addValueBase(value);
        extremes = {std::min(extremes.x, value), std::max(extremes.y, value)};
        updateGeometry();
    }

    void setPosition(Vec2 pos)
    {
        position = pos;
        transform = {};
        transform.translate(pos);
    }

    void updateGeometry()
    {
        // Compute width
        uint32_t const slice_size      = data.getCount();
        float const    available_width = size.x - to<float>(slice_size - 1) * space_x;
        float const    bar_width       = available_width / to<float>(slice_size);

        // Compute height coef to remap values into [0, size.y]
        float const height_coef = size.y / (extremes.y - extremes.x);
        float const zero_y      = size.y - extremes.x * height_coef;

        // Update array sizes
        va_bar.resize(slice_size);
        va_lines.resize(slice_size);

        // Update geometry
        data.foreach([&](uint32_t const i, float const v) {
            float const height = v * height_coef;
            float const x      = to<float>(i) * (bar_width + space_x);
            float const y      = zero_y - height;
            va_bar.setVertex0Position(i, {x            , zero_y});
            va_bar.setVertex0Position(i, {x            ,      y});
            va_bar.setVertex0Position(i, {x + bar_width,      y});
            va_bar.setVertex0Position(i, {x + bar_width, zero_y});

            sf::Color const color = {accent_color.r, accent_color.g, accent_color.b, 50};
            va_bar.setQuadColor(i, color);

            float const header_size = std::min(header_height, std::abs(height));
            float const header_dir  = (height > 0.0f) ? -1.0f : 1.0f;
            va_lines.setVertex0Position(i, {x            , y + header_dir * header_size});
            va_lines.setVertex0Position(i, {x            ,                            y});
            va_lines.setVertex0Position(i, {x + bar_width,                            y});
            va_lines.setVertex0Position(i, {x + bar_width, y + header_dir * header_size});
            va_lines.setQuadColor(i, accent_color);
        });
    }

    void setColor(sf::Color const color)
    {
        accent_color = color;
    }

    void clear()
    {
        data.clear();
        samples_count = 0;
        extremes = {0.0f, 0.0f};
        va_bar.clear();
        va_lines.clear();
    }

    [[nodiscard]]
    uint32_t getGlobalValueIndex(uint32_t data_buffer_idx) const
    {
        return data_buffer_idx + ((samples_count < data.max_values_count) ? 0 : (samples_count - data.max_values_count));
    }

    [[nodiscard]]
    float getScaledY(float y) const
    {
        float const width = extremes.y - extremes.x;
        float const normalized_y = (y - extremes.x) / width;
        float const scaled_y = normalized_y * size.y;
        return size.y - scaled_y + position.y;
    }
};
