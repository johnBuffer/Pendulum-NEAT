#pragma once
#include <functional>
#include "engine/common/chart/line_chart.hpp"
#include "./card.hpp"


struct GraphWidget
{
    float const background_radius = 20.0f;
    float const title_height      = 10.0f;
    Vec2 const  padding           = Vec2{background_radius, 1.5f * background_radius};
    float const outline           = 5.0f;
    Vec2  const outline_vec       = {outline, outline};
    sf::Color   color             = sf::Color::White;

    Vec2       size;
    Vec2       position;
    LineChart  chart;
    Card       background;
    Card       outline_background;
    sf::Font*  font;
    sf::Text   title;
    sf::Text   current_value;
    float      font_scale = 1.0f;

    sf::VertexArray va_scale;
    sf::Color const scale_color  = {150, 150, 150};
    float           height_round = 10.0f;
    Vec2            inner_size;
    float           last_value = 0.0f;
    uint32_t        tick_x_period = 20;

    std::function<std::string(uint32_t)> label_callback;
    std::function<std::string(float)>    current_value_callback;

    explicit
    GraphWidget(Vec2 size_, Vec2 position_ = {})
        : size{size_}
        , position{position_}
        , background{size - 2.0f * outline_vec, background_radius, {50, 50, 50}}
        , outline_background{size, background_radius + outline, color}
        , chart{Vec2{size.x, size.y - title_height} - 2.0f * (padding + outline_vec)}
        , font{&pez::resources::getFont("font")}
        , va_scale{sf::PrimitiveType::Lines}
        , inner_size{size.x - padding.x - outline, size.y - padding.y - outline - title_height}
    {
        title.setString("Title");
        title.setFont(*font);
        title.setCharacterSize(24);
        title.setFillColor(sf::Color::White);

        current_value.setFont(*font);
        current_value.setCharacterSize(20);
        current_value.setFillColor(sf::Color::White);

        setPosition(position);

        label_callback = [](uint32_t i) {
            return toString(i);
        };

        current_value_callback = [](float x) {
            return toString(x);
        };
    }

    void setSize(Vec2 size_)
    {
        size = size_;
        background = {size - 2.0f * outline_vec, background_radius, {50, 50, 50}};
        outline_background = {size, background_radius + outline, color};
        chart = LineChart(Vec2{size.x, size.y - title_height} - 2.0f * (padding + outline_vec));
        chart.setColor(color);
        inner_size = {size.x - padding.x - outline, size.y - padding.y - outline - title_height};
    }

    void setColor(sf::Color c)
    {
        color = c;
        chart.setColor(c);
        outline_background.setColor(c);
    }

    void clear()
    {
        chart.clear();
    }

    void addValue(float v)
    {
        last_value = v;
        chart.addValue(v);
    }

    void setPosition(Vec2 position_)
    {
        position                    = position_;
        outline_background.position = position;
        background.position         = position + outline_vec;
        chart.position              = background.position + padding + Vec2{0.0f, title_height};

        title.setPosition(position + Vec2{outline + 0.75f * padding.x, 0.35f * padding.y});
    }

    void render(pez::render::Context& context)
    {
        sf::Text scale_label;
        scale_label.setFont(*font);
        scale_label.setCharacterSize(18);
        scale_label.setFillColor(scale_color);
        scale_label.setScale(font_scale, font_scale);

        // Render background
        outline_background.renderHud(context);
        background.renderHud(context);

        // Render scale-X
        va_scale.clear();
        uint32_t const count = chart.values.getCount();
        float const dx = chart.size.x / float(count - 1);
        chart.values.foreach([&](uint32_t i, float v) {
            float const x = to<float>(i) * dx + position.x + padding.x + outline;

            uint32_t const current_i = chart.getGlobalValueIndex(i);
            if (current_i % tick_x_period == 0) {
                sf::Vertex vertex{};
                vertex.color = scale_color;
                vertex.position = {x, position.y + padding.y + title_height + outline};
                va_scale.append(vertex);
                vertex.position = {x, position.y + size.y - padding.y - outline};
                va_scale.append(vertex);

                scale_label.setString(label_callback(current_i));
                float const label_width = scale_label.getGlobalBounds().width;
                scale_label.setPosition(x - label_width * 0.5f, position.y + size.y - padding.y - outline);
                context.drawDirect(scale_label);
            }
        });

        // Render scale-Y
        uint32_t const tick_count  = 4;
        float const    y_width      = std::abs(chart.extremes.y - chart.extremes.x);
        float const    tick_height_raw = y_width / float(tick_count);
        float const    tick_height = std::max(1.0f, std::ceil(tick_height_raw / height_round)) * height_round;
        uint32_t const ticks = to<uint32_t>(y_width / tick_height) + 1;
        float const lower_tick = to<int32_t>(chart.extremes.x / tick_height) * tick_height;
        for (uint32_t i{0}; i < ticks; ++i) {
            sf::Vertex vertex{};
            vertex.color = scale_color;
            float const y = chart.getScaledY(lower_tick + to<float>(i) * tick_height);
            if (y >= chart.position.y) {
                vertex.position = {position.x + padding.x + outline, y};
                va_scale.append(vertex);
                vertex.position = {position.x + inner_size.x, y};
                va_scale.append(vertex);
            }
        }

        context.drawDirect(va_scale);

        // Render chart
        chart.render(context);

        for (uint32_t i{0}; i < ticks; ++i) {
            float const y = chart.getScaledY(lower_tick + to<float>(i) * tick_height);
            if (y >= chart.position.y) {
                scale_label.setString(toString(lower_tick + to<float>(i) * tick_height, y_width < 10.0f));
                auto const bounds = scale_label.getGlobalBounds();
                scale_label.setPosition(position.x + size.x - padding.x - bounds.width - 5.0f, y);
                context.drawDirect(scale_label);
            }
        }

        if (chart.values.getCount()) {
            current_value.setScale(font_scale, font_scale);
            current_value.setString(current_value_callback(last_value));
            current_value.setPosition(position.x + size.x - outline - padding.x - current_value.getGlobalBounds().width, title.getPosition().y);
            context.drawDirect(current_value);
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