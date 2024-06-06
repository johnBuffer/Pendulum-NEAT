#pragma once
#include <SFML/Graphics.hpp>
#include "engine/common/vec.hpp"
#include "engine/common/math.hpp"
#include "engine/render/render_context.hpp"


struct Card
{
    // Struct to share configuration between create and update functions
    struct Conf
    {
        uint32_t quality = 64;
        float    da      = Math::ConstantF32::Pi * 0.5f / static_cast<float>(quality - 1);
    };

    sf::VertexArray va;
    sf::VertexArray va_shadow;

    Vec2            position;
    Vec2            size;
    float           corner_radius;
    float           shadow_size   = 8.0f;

    Card(Vec2 size_, float corner_radius_, sf::Color color)
        : va{sf::PrimitiveType::TriangleFan}
        , va_shadow{sf::PrimitiveType::TriangleStrip}
        , size{size_}
        , corner_radius{corner_radius_}
    {
        updateCard();
        setColor(color);
    }

    void setColor(sf::Color color)
    {
        uint64_t const vertex_count = va.getVertexCount();
        for (uint64_t i{vertex_count}; i--;) {
            va[i].color = color;
        }
    }

    void updateCard()
    {
        generateGeometry(va);
        generateGeometryShadow(va_shadow, {0, 0, 0, 50}, {0, 0, 0, 0}, shadow_size);
    }

    void setShadowSize(float size_)
    {
        shadow_size = size_;
        updateCard();
    }

    void renderHud(pez::render::Context& context)
    {
        sf::Transform transform;
        transform.translate(position);
        if (shadow_size > 0.0f) {
            context.drawDirect(va_shadow, transform);
        }
        context.drawDirect(va, transform);
    }

    void render(pez::render::Context& context)
    {
        sf::Transform transform;
        transform.translate(position);
        if (shadow_size > 0.0f) {
            context.draw(va_shadow, transform);
        }
        context.draw(va, transform);
    }

    void setWidth(float width)
    {
        size.x = width;
        updateCard();
    }

    void setSize(Vec2 size_)
    {
        size = size_;
        updateCard();
    }

    void generateGeometry(sf::VertexArray& vertex_array, float out = 0.0f)
    {
        const Conf conf;
        const float radius     = std::min(std::min(corner_radius, size.x * 0.5f), size.y * 0.5f);
        const float radius_out = radius + out;

        vertex_array.resize(4 * conf.quality + 2);
        uint32_t global_index = 0;

        // Center
        vertex_array[global_index++].position = size * 0.5f;

        // Top left
        {
            const Vec2 center = Vec2{radius, radius};
            for (uint32_t i(0); i < conf.quality; ++i) {
                const auto fi = static_cast<float>(i);
                const float angle = Math::ConstantF32::Pi + fi * conf.da;
                vertex_array[global_index++].position = center + radius_out * Vec2{cos(angle), sin(angle)};
            }
        }
        // Top right
        {
            const Vec2 center = Vec2{size.x - radius, radius};
            for (uint32_t i(0); i < conf.quality; ++i) {
                const auto fi = static_cast<float>(i);
                const float angle = Math::ConstantF32::Pi * 1.5f + fi * conf.da;
                vertex_array[global_index++].position = center + radius_out * Vec2{cos(angle), sin(angle)};
            }
        }
        // Bottom right
        {
            const Vec2 center = Vec2{size.x, size.y} - Vec2{radius, radius};
            for (uint32_t i(0); i < conf.quality; ++i) {
                const auto fi = static_cast<float>(i);
                const float angle = Math::ConstantF32::Pi * 2.0f + fi * conf.da;
                vertex_array[global_index++].position = center + radius_out * Vec2{cos(angle), sin(angle)};
            }
        }
        // Bottom left
        {
            const Vec2 center = Vec2{radius, size.y - radius};
            for (uint32_t i(0); i < conf.quality; ++i) {
                const auto fi = static_cast<float>(i);
                const float angle = Math::ConstantF32::Pi * 2.5f + fi * conf.da;
                vertex_array[global_index++].position = center + radius_out * Vec2{cos(angle), sin(angle)};
            }
        }

        vertex_array[global_index++].position = Vec2{-out, radius};
    }

    void generateGeometryShadow(sf::VertexArray& vertex_array, sf::Color color_in, sf::Color color_out, float out = 0.0f)
    {
        const Conf  conf;
        const float radius     = std::min(std::min(corner_radius, size.x * 0.5f), size.y * 0.5f);
        const float radius_out = radius + out;

        vertex_array.resize(8 * conf.quality + 2);
        uint32_t global_index = 0;

        // Top left
        {
            const Vec2 center = Vec2{radius, radius};
            for (uint32_t i(0); i < conf.quality; ++i) {
                const auto fi = static_cast<float>(i);
                const float angle = Math::ConstantF32::Pi + fi * conf.da;
                vertex_array[global_index  ].color    = color_in;
                vertex_array[global_index++].position = center + radius     * Vec2{cos(angle), sin(angle)};
                vertex_array[global_index  ].color    = color_out;
                vertex_array[global_index++].position = center + radius_out * Vec2{cos(angle), sin(angle)};

            }
        }
        // Top right
        {
            const Vec2 center = Vec2{size.x - radius, radius};
            for (uint32_t i(0); i < conf.quality; ++i) {
                const auto fi = static_cast<float>(i);
                const float angle = Math::ConstantF32::Pi * 1.5f + fi * conf.da;
                vertex_array[global_index  ].color    = color_in;
                vertex_array[global_index++].position = center + radius     * Vec2{cos(angle), sin(angle)};
                vertex_array[global_index  ].color    = color_out;
                vertex_array[global_index++].position = center + radius_out * Vec2{cos(angle), sin(angle)};
            }
        }
        // Bottom right
        {
            const Vec2 center = Vec2{size.x, size.y} - Vec2{radius, radius};
            for (uint32_t i(0); i < conf.quality; ++i) {
                const auto fi = static_cast<float>(i);
                const float angle = Math::ConstantF32::Pi * 2.0f + fi * conf.da;
                vertex_array[global_index  ].color    = color_in;
                vertex_array[global_index++].position = center + radius     * Vec2{cos(angle), sin(angle)};
                vertex_array[global_index  ].color    = color_out;
                vertex_array[global_index++].position = center + radius_out * Vec2{cos(angle), sin(angle)};
            }
        }
        // Bottom left
        {
            const Vec2 center = Vec2{radius, size.y - radius};
            for (uint32_t i(0); i < conf.quality; ++i) {
                const auto fi = static_cast<float>(i);
                const float angle = Math::ConstantF32::Pi * 2.5f + fi * conf.da;
                vertex_array[global_index  ].color    = color_in;
                vertex_array[global_index++].position = center + radius     * Vec2{cos(angle), sin(angle)};
                vertex_array[global_index  ].color    = color_out;
                vertex_array[global_index++].position = center + radius_out * Vec2{cos(angle), sin(angle)};
            }
        }

        vertex_array[global_index  ].color    = color_in;
        vertex_array[global_index++].position = {0.0f, radius};
        vertex_array[global_index  ].color    = color_out;
        vertex_array[global_index++].position = {-out, radius};
    }
};
