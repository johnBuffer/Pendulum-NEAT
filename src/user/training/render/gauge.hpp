#pragma once

#include "user/common/render/card.hpp"


struct Gauge
{
    float padding           = 2.0f;
    float outline_thickness = 2.0f;
    float text_padding      = 10.0f;

    Vec2  size;
    Vec2  position;

    EmptyCard outline;
    Card      value;

    sf::Text title;
    sf::Font& font;

    Gauge()
        : outline{{}, 0.0f, sf::Color::White}
        , value{{}, 0.0f, sf::Color::White}
        , font{pez::resources::getFont("font")}
    {
        title.setFont(font);
        title.setCharacterSize(16);
        title.setFillColor({150, 150, 150});
        title.setString("Title");
    }

    void render(pez::render::Context& context)
    {
        context.drawDirect(title);
        value.renderHud(context);
        outline.renderHud(context);
    }

    void setSize(Vec2 size_)
    {
        size = size_;
        outline = {size, size.y * 0.5f, sf::Color::White};
        outline.setThickness(outline_thickness);

        float const total_padding = outline_thickness + padding;
        value = {outline.size - 2.0f * Vec2{total_padding, total_padding}, size.y * 0.5f - total_padding, sf::Color::White};
    }

    void setPosition(Vec2 position_)
    {
        position = position_;
        title.setPosition(position);
        float const title_height = title.getGlobalBounds().height + text_padding;
        outline.position = position + Vec2{0.0f, title_height};

        float const total_padding = outline_thickness + padding;
        value.position = position + Vec2{total_padding, total_padding + title_height};
    }

    void setRatio(float ratio)
    {
        float const total_padding = outline_thickness + padding;
        value.setWidth(ratio * (outline.size.x - 2.0f * total_padding));
    }

    void setTitle(std::string const& string)
    {
        title.setString(string);
        setPosition(position);
    }
};
