#pragma once
#include "engine/common/vec.hpp"


namespace pez::render
{
    void setFocus(Vec2 focus);
    void setZoom(float zoom);
    Vec2 getFocus();
    float getZoom();

    void clear(sf::Color color);
    void draw(sf::Drawable const& drawable);
    void display();
    Vec2 getRenderSize();
    void moveFocusRelative(Vec2 d);
}