#pragma once
#include "engine/render/vertex_array/vertex_array.hpp"


namespace gui
{
struct Utils
{
    static void generateFrame(VertexArray& va, Vec2 size, float outline, Color frame_color = {200, 200, 200})
    {
        const Vec2 size_inner = size - 2.0f * outline;

        va.initializeBuffers(GeometryType::Quads, 20);
        va.setVerticesColor(frame_color);
        // Top
        va[0].position = {0.0f  , 0.0f};
        va[1].position = {size.x, 0.0f};
        va[2].position = {size.x, outline};
        va[3].position = {0.0f  , outline};
        // Bottom
        va[4].position = {0.0f  , size.y - outline};
        va[5].position = {size.x, size.y - outline};
        va[6].position = {size.x, size.y};
        va[7].position = {0.0f  , size.y};
        // Left
        va[ 8].position = {0.0f   , outline};
        va[ 9].position = {outline, outline};
        va[10].position = {outline, size.y - outline};
        va[11].position = {0.0f   , size.y - outline};
        // Right
        va[12].position = {size.x - outline, outline};
        va[13].position = {size.x          , outline};
        va[14].position = {size.x          , size.y - outline};
        va[15].position = {size.x - outline, size.y - outline};

        va[16].position = {outline               , outline};
        va[17].position = {outline + size_inner.x, outline};
        va[18].position = {outline + size_inner.x, outline + size_inner.y};
        va[19].position = {outline               , outline + size_inner.y};
        va[16].color = Color::Black;
        va[17].color = Color::Black;
        va[18].color = Color::Black;
        va[19].color = Color::Black;
        va.updateGeometry();
    }

    static void setBackgroundColor(VertexArray& va, Color color)
    {
        va[16].color = color;
        va[17].color = color;
        va[18].color = color;
        va[19].color = color;
        va.updateVerticesOnly();
    }
};
}
