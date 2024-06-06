#pragma once
#include "user/common/render/utils.hpp"


struct Cart
{
    sf::Texture* wheel_texture;

    Vec2 position      = {};
    Vec2 position_last = {};

    Vec2  offset  = {25.0f, 1.0f};
    float delta_x = 0.0f;

    sf::VertexArray va_lines;
    sf::Color color = sf::Color::White;

    Cart()
        : wheel_texture{&pez::resources::getTexture("wheel")}
        , va_lines{sf::PrimitiveType::Quads, 12}
    {}

    void setPosition(Vec2 position_)
    {
        position_last = position;
        position      = position_;

        delta_x += (position.x - position_last.x);
    }

    void render(pez::render::Context& context)
    {
        float const radius       = 12.0f;
        float const bottom_scale = 0.75f;

        Vec2 wheel_1_position = position + Vec2{ offset.x, offset.y}         + Vec2{0.0f, radius};
        Vec2 wheel_2_position = position + Vec2{-offset.x, offset.y}         + Vec2{0.0f, radius};
        Vec2 wheel_3_position = position + Vec2{ 1.5f * offset.x, -offset.y} - Vec2{0.0f, radius * bottom_scale};
        Vec2 wheel_4_position = position + Vec2{-1.5f * offset.x, -offset.y} - Vec2{0.0f, radius * bottom_scale};

        float const width = 32.0f;
        common::Utils::generateLine(va_lines, 0, position - Vec2{width, 0.0f}, position + Vec2{width, 0.0f}, 6.0f, color);

        common::Utils::generateLine(va_lines,  4, wheel_1_position, wheel_3_position, 8.0f, color);
        common::Utils::generateLine(va_lines,  8, wheel_2_position, wheel_4_position, 8.0f, color);
        context.draw(va_lines);

        float const radius_in{3.0f};
        sf::CircleShape object_in{radius_in};
        object_in.setOrigin(radius_in, radius_in);
        object_in.setPosition(position);
        object_in.setFillColor(color);
        context.draw(object_in);

        sf::RectangleShape wheel{2.0f * Vec2{radius, radius}};
        float const angle = Math::radToDeg(delta_x / radius);
        wheel.setTexture(wheel_texture);
        wheel.setFillColor(color);
        wheel.setOrigin(radius, radius);

        wheel.setPosition(wheel_1_position);
        wheel.setRotation(-angle);
        context.draw(wheel);

        wheel.setPosition(wheel_2_position);
        wheel.setRotation(-angle);
        context.draw(wheel);

        // Small wheels
        wheel.setScale(bottom_scale, bottom_scale);
        wheel.setPosition(wheel_3_position);
        wheel.setRotation(angle / bottom_scale);
        context.draw(wheel);

        wheel.setPosition(wheel_4_position);
        wheel.setRotation(angle / bottom_scale);
        context.draw(wheel);
    }
};