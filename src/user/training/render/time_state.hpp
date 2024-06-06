#pragma once
#include "engine/engine.hpp"

#include "user/common/render/card.hpp"


struct TimeState
{
    float const outline_thickness = 5.0f;
    Vec2 const size = {260.0f, 70.0f};

    sf::Color const speed_off_color{100, 100, 100};

    Card background;
    Card outline;

    sf::Text text_generation;
    sf::Text text_generation_count;

    TimeState(sf::Color accent_color)
        : background(size - 2.0f * Vec2{outline_thickness, outline_thickness}, 20.0f, sf::Color{50, 50, 50})
        , outline(size, 20.0f + outline_thickness, accent_color)
    {}

    void setFont(sf::Font& font)
    {
        text_generation.setFont(font);
        text_generation.setCharacterSize(24);
        text_generation.setFillColor(sf::Color::White);
        text_generation.setString("Generation");

        text_generation_count = text_generation;
        text_generation_count.setCharacterSize(40);
    }

    void setPosition(Vec2 pos)
    {
        Vec2 const offset                  = {outline_thickness, outline_thickness};
        Vec2 const generation_offset       = {outline_thickness + 5.0f, outline_thickness + 15.0f};
        Vec2 const generation_count_offset = {outline_thickness + 152.0f, outline_thickness + 5.0f};

        text_generation.setPosition(pos + offset + generation_offset);
        text_generation_count.setPosition(pos + generation_count_offset);

        outline.position    = pos;
        background.position = pos + offset;
    }

    void render(pez::render::Context& context)
    {
        outline.renderHud(context);
        background.renderHud(context);

        context.drawDirect(text_generation);
        context.drawDirect(text_generation_count);
    }

    void setIteration(uint32_t iteration)
    {
        auto const prefix = std::string{iteration < 10 ? "0" : ""} +
                (iteration <  100 ? "0" : "") +
                (iteration < 1000 ? "0" : "");
        text_generation_count.setString(prefix + toString(iteration));
    }
};
