#pragma once
#include "engine/common/vec.hpp"

namespace pbd
{

struct Vertex
{
    float mass          = 1.0f;
    float w             = 1.0f;
    Vec2  position      = {};
    Vec2  position_last = {};
    Vec2  velocity      = {};

    Vertex() = default;

    explicit
    Vertex(Vec2 pos)
        : position{pos}
        , position_last{pos}
        , velocity{0.0f, 0.0f}
    {

    }
};

}
