#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

#include "engine/common/math.hpp"


namespace common
{
struct Utils
{
    static void generateLine(sf::VertexArray& va, uint32_t index, sf::Vector2f point_1, sf::Vector2f point_2, float width, sf::Color color, float offset = 0.0f)
    {
        sf::Vector2f const p1_p2  = point_2 - point_1;
        float const        length = std::sqrt(p1_p2.x * p1_p2.x + p1_p2.y * p1_p2.y);
        sf::Vector2f const v      = p1_p2 / length;
        sf::Vector2f const n      = {-v.y, v.x};

        Vec2 const offset_v = v * (offset * 0.5f);
        Vec2 const normal_v = n * 0.5f * width;

        va[index + 0].position = point_1 + offset_v + normal_v;
        va[index + 1].position = point_2 - offset_v + normal_v;
        va[index + 2].position = point_2 - offset_v - normal_v;
        va[index + 3].position = point_1 + offset_v - normal_v;

        va[index + 0].color = color;
        va[index + 1].color = color;
        va[index + 2].color = color;
        va[index + 3].color = color;
    }

    static void generateBezier(sf::VertexArray& va, Vec2 pt1, Vec2 pt2, Vec2 pt3, uint32_t pts_count, sf::Color color, float offset = 0.0f)
    {
        uint32_t const pts_bezier = pts_count - 1;
        float const    pts_delta  = 1.0f / static_cast<float>(pts_bezier - 1);

        auto const m1  = pt1 - pt2;
        auto const m2  = pt3 - pt2;
        auto const m3  = MathVec2::normalize(MathVec2::length(m1) * m2 + MathVec2::length(m2) * m1);
        auto const mid = pt2 + offset * m3;

        auto const v1 = mid - pt1;
        auto const v2 = pt3 - mid;

        va[0].position = pt2;
        for (uint32_t i{0}; i < pts_bezier; ++i) {
            float const ratio = static_cast<float>(i) * pts_delta;
            auto const b1 = pt1 + ratio * v1;
            auto const b2 = mid + ratio * v2;
            auto const bi = b2 - b1;
            va[i + 1].position = b1 + ratio * bi;
        }

        setVertexArrayColor(va, color);
    }

    static void generateCircle(sf::VertexArray& va, float radius, uint32_t quality, sf::Color color)
    {
        va[0].position = {0.0f, 0.0f};
        float const da = Math::Constant<float>::TwoPi / static_cast<float>(quality - 2);
        for (uint32_t i{0}; i < quality - 1; ++i) {
            float const angle = static_cast<float>(i) * da;
            va[i + 1].position = {radius * cos(angle), radius * sin(angle)};
        }
        setVertexArrayColor(va, color);
    }

    static void setVertexArrayColor(sf::VertexArray& va, sf::Color color)
    {
        uint64_t const vertex_count = va.getVertexCount();
        for (uint64_t i{0}; i<vertex_count; ++i) {
            va[i].color = color;
        }
    }
};
}
