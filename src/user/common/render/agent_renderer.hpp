#pragma once
#include <SFML/Graphics.hpp>
#include "engine/engine.hpp"

#include "cart.hpp"
#include "user/common/configuration.hpp"
#include "user/common/agent.hpp"
#include "user/common/render/utils.hpp"


struct AgentRenderer
{
    enum class Mode
    {
        Solid,
        Ghost
    };

    uint8_t alpha = 50;

    sf::VertexArray va_links;
    Cart cart;

    AgentRenderer()
        : va_links{sf::PrimitiveType::Quads, conf::sim::segments_count * 4}
    {

    }

    [[nodiscard]]
    sf::Color getColor(Mode mode) const
    {
        if (mode == Mode::Solid) {
            return sf::Color::White;
        } else {
            return {255, 255, 255, alpha};
        }
    }

    [[nodiscard]]
    sf::Color getJointColor(Mode mode, uint32_t idx) const
    {
        uint8_t const a = (mode == Mode::Solid) ? 255 : alpha;
        switch (idx) {
            case 0:
                return {138, 177, 125, a};
            case 1:
                return {244, 162, 97, a};
            case 2:
                return {231, 111, 81, a};
        }
        return sf::Color::White;
    }

    void renderAgent(pez::render::Context& context, Agent const& agent, Mode mode)
    {
        sf::Color const color = getColor(mode);

        float const radius{10.0f};
        float const outline{2.0f};

        // Links
        uint32_t i{0};
        for (auto const& o : agent.system.objects) {
            common::Utils::generateLine(va_links,
                                        i * 4,
                                        Vec2{o.getWorldPosition(0)},
                                        Vec2{o.getWorldPosition(1)},
                                        4.0f,
                                        color,
                                        2.0f * (outline + radius));
            ++i;
        }
        context.draw(va_links);

        // Cart
        cart.color = color;
        cart.setPosition(Vec2{agent.system.drag_constraints[0].target});
        cart.render(context);

        // Objects
        i = 0;
        sf::CircleShape object{radius};
        object.setOrigin(radius, radius);
        object.setFillColor(getJointColor(mode, i));
        object.setOutlineColor(color);
        object.setOutlineThickness(outline);
        object.setPosition(cart.position);
        context.draw(object);
        for (auto const& o: agent.system.objects) {
            ++i;
            object.setPosition(Vec2{o.getWorldPosition(1)});
            object.setFillColor(getJointColor(mode, i));
            context.draw(object);
        }
    }
};