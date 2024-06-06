#pragma once
#include <functional>
#include "engine/common/chart/line_chart.hpp"
#include "./card.hpp"


struct MultiGraphWidget
{
    float const background_radius = 20.0f;
    float const title_height      = 10.0f;
    Vec2 const  padding           = Vec2{background_radius, 1.5f * background_radius};
    float const outline           = 5.0f;
    Vec2  const outline_vec       = {outline, outline};
    sf::Color   color             = sf::Color::White;

    Vec2 extremes_x = {-1.0f, 1.0f};
    Vec2 extremes_y = {-1.0f, 1.0f};

    Vec2       size;
    Vec2       position;
    Card       background;
    Card       outline_background;
    sf::Font*  font;
    sf::Text   title;
    float      font_scale = 1.0f;

    std::vector<Vec2>        points;
    std::vector<SmoothFloat> width;
    sf::VertexArray          va_line;

    explicit
    MultiGraphWidget(Vec2 size_, Vec2 position_ = {})
        : size{size_}
        , position{position_}
        , background{size - 2.0f * outline_vec, background_radius, {50, 50, 50}}
        , outline_background{size, background_radius + outline, color}
        , font{&pez::resources::getFont("font")}
        , va_line{sf::PrimitiveType::TriangleStrip}
    {
        title.setString("Title");
        title.setFont(*font);
        title.setCharacterSize(24);
        title.setFillColor(sf::Color::White);

        setPosition(position);
    }

    void setSize(Vec2 size_)
    {
        size = size_;
        background = {size - 2.0f * outline_vec, background_radius, {50, 50, 50}};
        outline_background = {size, background_radius + outline, color};
    }

    void setColor(sf::Color c)
    {
        color = c;
        outline_background.setColor(c);
    }

    void clear()
    {
        points.clear();
        width.clear();
        extremes_x = {-1.0f, 1.0f};
        extremes_y = {-1.0f, 1.0f};
    }

    void addPoint(Vec2 pt)
    {
        extremes_x = {std::min(extremes_x.x, pt.x), std::max(extremes_x.y, pt.x)};
        extremes_y = {std::min(extremes_y.x, pt.y), std::max(extremes_y.y, pt.y)};
        points.push_back(pt);
        auto& w = width.emplace_back();
        w.setValueInstant(4.0f);
        w = 1.0f;
    }

    [[nodiscard]]
    Vec2 remap_point(Vec2 pt) const
    {
        Vec2 const total_margin = outline_vec + padding;
        Vec2 const inner_size = size - 2.0f * total_margin - Vec2(0.0f, title_height);
        float const x_width = extremes_x.y - extremes_x.x;
        float const y_width = extremes_y.y - extremes_y.x;
        float const x = (pt.x - extremes_x.x) / x_width * inner_size.x;
        float const y = (pt.y - extremes_y.x) / y_width * inner_size.y + title_height;
        return Vec2{x, y} + total_margin;
    }

    void setPosition(Vec2 position_)
    {
        position                    = position_;
        outline_background.position = position;
        background.position         = position + outline_vec;

        title.setPosition(position + Vec2{outline + 0.75f * padding.x, 0.35f * padding.y});
    }

    void render(pez::render::Context& context)
    {
        // Background
        outline_background.renderHud(context);
        background.renderHud(context);

        // Line
        sf::Transform transform;
        transform.translate(position);
        va_line.resize(2 * points.size() - 2);

        if (points.size() > 1) {
            Vec2 last = remap_point(points[0]);
            uint32_t i{0};
            for (auto const& pt: points) {
                if (i > 0) {
                    Vec2 const current = remap_point(pt);
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
            context.drawDirect(va_line, transform);
        }

        // Render title
        title.setScale(font_scale, font_scale);
        context.drawDirect(title);
    }

    void setTitle(std::string const& title_)
    {
        title.setString(title_);
    }
};