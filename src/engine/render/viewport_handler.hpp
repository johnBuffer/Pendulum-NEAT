#pragma once
#include <SFML/Graphics.hpp>
#include "engine/common/vec.hpp"


struct ViewportHandler
{
    struct State
    {
        Vec2          center;
        Vec2          offset;
        float          zoom;
        float          scale;
        bool          clicking;
        Vec2          mouse_position;
        Vec2          mouse_world_position;
        sf::Transform transform;

        explicit
        State(Vec2 render_size, const float base_zoom = 1.0f)
            : center(render_size.x * 0.5f, render_size.y * 0.5f)
            , offset(center / base_zoom)
            , zoom(base_zoom)
            , scale(1.0f)
            , clicking(false)
        {
        }

        void updateState()
        {
            transform = {};
            transform.translate(center);
            transform.scale(zoom, zoom);
            transform.translate(offset);
        }

        void updateMousePosition(Vec2 new_position)
        {
            mouse_position = new_position;
            mouse_world_position = offset + (mouse_position * scale - center) / (zoom * scale);
        }

        void setCenter(Vec2 c)
        {
            center = c;
            updateState();
        }
    };

    State state;

    explicit
    ViewportHandler(Vec2 size = {}, float scale = 1.0f)
        : state(size)
    {
        state.scale = scale;
        state.updateState();
    }

    void addOffset(Vec2 v)
    {
        state.offset += v / state.zoom;
        state.updateState();
    }

    void zoom(float f)
    {
        state.zoom *= f;
        state.updateState();
    }

    void wheelZoom(float w)
    {
        if (w != 0.0f) {
            const float zoom_amount = 1.2f;
            const float delta = w > 0 ? zoom_amount : 1.0f / zoom_amount;
            zoom(delta);
        }
    }

    void reset()
    {
        state.zoom = 1.0f;
        setFocus(state.center);
    }

    [[nodiscard]]
    const sf::Transform& getTransform() const
    {
        return state.transform;
    }

    void click(Vec2 relative_click_position)
    {
        state.mouse_position = relative_click_position;
        state.clicking       = true;
    }

    void unclick()
    {
        state.clicking = false;
    }

    void setMousePosition(Vec2 new_mouse_position)
    {
        if (state.clicking) {
            addOffset(new_mouse_position - state.mouse_position);
        }
        state.updateMousePosition(new_mouse_position);
    }

    void setFocus(Vec2 focus_position)
    {
        state.offset = -focus_position;
        state.updateState();
    }

    void setZoom(float zoom)
    {
        state.zoom = zoom;
        state.updateState();
    }

    Vec2 getMouseWorldPosition() const
    {
        return state.mouse_world_position;
    }

    Vec2 getScreenCoords(Vec2 world_pos) const
    {
        return {};//state.transform.transformPoint(world_pos);
    }
};
