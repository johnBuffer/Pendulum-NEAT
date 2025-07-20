#pragma once
#include <SFML/Graphics.hpp>
#include "./math.hpp"

namespace pez
{

struct QuadVertexArray
{
private:
    sf::VertexArray m_va;

public:
    explicit
    QuadVertexArray(size_t const quad_count = 0)
        : m_va{sf::PrimitiveType::Triangles, getQuadVertexCount(quad_count)}
    {

    }

    [[nodiscard]]
    static size_t getQuadVertexCount(size_t const quad_count)
    {
        return quad_count * 6;
    }

    void resize(size_t const quad_count)
    {
        m_va.resize(getQuadVertexCount(quad_count));
    }

    // ----- POSITION -----
    void setVertex0Position(size_t const quad_index, sf::Vector2f const position)
    {
        size_t const vertex_index = quad_index * 6;
        m_va[vertex_index    ].position = position;
        m_va[vertex_index + 4].position = position;
    }

    void setVertex1Position(size_t const quad_index, sf::Vector2f const position)
    {
        size_t const vertex_index = quad_index * 6;
        m_va[vertex_index + 1].position = position;
    }

    void setVertex2Position(size_t const quad_index, sf::Vector2f const position)
    {
        size_t const vertex_index = quad_index * 6;
        m_va[vertex_index + 2].position = position;
        m_va[vertex_index + 3].position = position;
    }

    void setVertex3Position(size_t const quad_index, sf::Vector2f const position)
    {
        size_t const vertex_index = quad_index * 6;
        m_va[vertex_index + 5].position = position;
    }

    // ----- COLOR -----
    void setVertex0Color(size_t const quad_index, sf::Color const& color)
    {
        size_t const vertex_index = quad_index * 6;
        m_va[vertex_index    ].color = color;
        m_va[vertex_index + 4].color = color;
    }

    void setVertex1Color(size_t const quad_index, sf::Color const& color)
    {
        size_t const vertex_index = quad_index * 6;
        m_va[vertex_index + 1].color = color;
    }

    void setVertex2Color(size_t const quad_index, sf::Color const& color)
    {
        size_t const vertex_index = quad_index * 6;
        m_va[vertex_index + 2].color = color;
        m_va[vertex_index + 3].color = color;
    }

    void setVertex3Color(size_t const quad_index, sf::Color const& color)
    {
        size_t const vertex_index = quad_index * 6;
        m_va[vertex_index + 5].color = color;
    }

    // ----- TEXTURE -----
    void setVertex0TextureCoord(size_t const quad_index, sf::Vector2f const coord)
    {
        size_t const vertex_index = quad_index * 6;
        m_va[vertex_index    ].texCoords = coord;
        m_va[vertex_index + 4].texCoords = coord;
    }

    void setVertex1TextureCoord(size_t const quad_index, sf::Vector2f const coord)
    {
        size_t const vertex_index = quad_index * 6;
        m_va[vertex_index + 1].texCoords = coord;
    }

    void setVertex2TextureCoord(size_t const quad_index, sf::Vector2f const coord)
    {
        size_t const vertex_index = quad_index * 6;
        m_va[vertex_index + 2].texCoords = coord;
        m_va[vertex_index + 3].texCoords = coord;
    }

    void setVertex3TextureCoord(size_t const quad_index, sf::Vector2f const coord)
    {
        size_t const vertex_index = quad_index * 6;
        m_va[vertex_index + 5].texCoords = coord;
    }

    [[nodiscard]]
    sf::VertexArray const& asVertexArray() const
    {
        return m_va;
    }

    [[nodiscard]]
    sf::Drawable const& asDrawable() const
    {
        return m_va;
    }

    void mapTextureCoord(size_t const quad_idx, sf::Vector2f const top_left, sf::Vector2f const bot_right)
    {
        setVertex0TextureCoord(quad_idx, top_left);
        setVertex1TextureCoord(quad_idx, {bot_right.x, top_left.y});
        setVertex2TextureCoord(quad_idx, bot_right);
        setVertex3TextureCoord(quad_idx, {top_left.x, bot_right.y});
    }

    void createQuad(size_t const quad_idx, sf::Vector2f const pos, sf::Vector2f const dir, sf::Vector2f const size)
    {
        sf::Vector2f const d = dir * size.x;
        sf::Vector2f const n = MathVec2::normal(dir) * (size.y * 0.5f);
        setVertex0Position(quad_idx, pos + n    );
        setVertex1Position(quad_idx, pos + n + d);
        setVertex2Position(quad_idx, pos - n + d);
        setVertex3Position(quad_idx, pos - n    );
    }

    void setQuadColor(size_t const quad_idx, sf::Color const color)
    {
        setVertex0Color(quad_idx, color);
        setVertex1Color(quad_idx, color);
        setVertex2Color(quad_idx, color);
        setVertex3Color(quad_idx, color);
    }

    void setQuadEmpty(size_t const quad_idx)
    {
        setVertex0Position(quad_idx, {});
        setVertex1Position(quad_idx, {});
        setVertex2Position(quad_idx, {});
        setVertex3Position(quad_idx, {});
    }

    void setAllQuadColor(sf::Color const color)
    {
        size_t const vertex_count = m_va.getVertexCount();
        for (size_t i = 0; i < vertex_count; ++i) {
            m_va[i].color = color;
        }
    }

    void createRectangle(size_t const quad_idx, sf::Vector2f const size, sf::Transform const& transform)
    {
        sf::Vector2f const half_size = size * 0.5f;
        setVertex0Position(quad_idx, transform * sf::Vector2f{-half_size.x, -half_size.y});
        setVertex1Position(quad_idx, transform * sf::Vector2f{ half_size.x, -half_size.y});
        setVertex2Position(quad_idx, transform * sf::Vector2f{ half_size.x,  half_size.y});
        setVertex3Position(quad_idx, transform * sf::Vector2f{-half_size.x,  half_size.y});
    }

    void createAlignedRectangle(size_t const quad_idx, sf::Vector2f const size, sf::Vector2f const pos)
    {
        sf::Vector2f const half_size = size * 0.5f;
        setVertex0Position(quad_idx, pos + sf::Vector2f{-half_size.x, -half_size.y});
        setVertex1Position(quad_idx, pos + sf::Vector2f{ half_size.x, -half_size.y});
        setVertex2Position(quad_idx, pos + sf::Vector2f{ half_size.x,  half_size.y});
        setVertex3Position(quad_idx, pos + sf::Vector2f{-half_size.x,  half_size.y});
    }

    void createLine(uint32_t const index, sf::Vector2f const point_1, sf::Vector2f const point_2, float const width, sf::Color const color, float const offset = 0.0f)
    {
        sf::Vector2f const p1_p2  = point_2 - point_1;
        float const        length = std::sqrt(p1_p2.x * p1_p2.x + p1_p2.y * p1_p2.y);
        sf::Vector2f const v      = p1_p2 / length;
        sf::Vector2f const n      = {-v.y, v.x};

        Vec2 const offset_v = v * (offset * 0.5f);
        Vec2 const normal_v = n * 0.5f * width;

        setVertex0Position(index, point_1 + offset_v + normal_v);
        setVertex1Position(index, point_2 - offset_v + normal_v);
        setVertex2Position(index, point_2 - offset_v - normal_v);
        setVertex3Position(index, point_1 + offset_v - normal_v);

        setQuadColor(index, color);
    }

    void clear()
    {
        m_va.clear();
    }
};

}