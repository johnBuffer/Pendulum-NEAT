#pragma once
#include "engine/common/vec.hpp"
#include "engine/common/event_manager.hpp"
#include <SFML/Graphics.hpp>

#include "viewport_handler.hpp"


namespace pez::render
{
class Context
{
public:
    Context()  = default;
    ~Context() = default;

    IVec2 m_size = {};

    void registerCallbacks(sfev::EventManager& event_manager, bool use_viewport_callbacks)
    {
        event_manager.addEventCallback(sf::Event::MouseMoved, [&](sfev::CstEv) {
            m_viewport_handler.setMousePosition(event_manager.getFloatMousePosition());
        });
        if (use_viewport_callbacks) {
            event_manager.addMousePressedCallback(sf::Mouse::Left, [&](sfev::CstEv) {
                m_viewport_handler.click(event_manager.getFloatMousePosition());
            });
            event_manager.addMouseReleasedCallback(sf::Mouse::Left, [&](sfev::CstEv) {
                m_viewport_handler.unclick();
            });
            event_manager.addEventCallback(sf::Event::MouseWheelScrolled, [&](sfev::CstEv e) {
                m_viewport_handler.wheelZoom(e.mouseWheelScroll.delta);
            });
        }
    }

    void setFocus(Vec2 focus)
    {
        m_viewport_handler.setFocus(focus);
    }

    void setZoom(float zoom)
    {
        m_viewport_handler.setZoom(zoom);
    }

    void clear(sf::Color color = sf::Color::Black)
    {
        m_window->clear(color);
    }

    void display()
    {
        m_window->display();
    }

    [[nodiscard]]
    IVec2 getRenderSize() const
    {
        return m_render_size;
    }

    void draw(sf::Drawable& drawable)
    {
        m_window->draw(drawable, m_viewport_handler.getTransform());
    }

    void draw(sf::Drawable& drawable, sf::Transform const& transform)
    {
        m_window->draw(drawable, m_viewport_handler.getTransform() * transform);
    }

    void draw(sf::Drawable& drawable, sf::RenderStates const& states)
    {
        sf::RenderStates final_states = states;
        final_states.transform = m_viewport_handler.getTransform() * states.transform;
        m_window->draw(drawable, final_states);
    }

    void drawDirect(sf::Drawable const& drawable, sf::BlendMode mode = sf::BlendNone)
    {
        m_window->draw(drawable);
    }

    void drawDirect(sf::Drawable& drawable, sf::Transform const& transform)
    {
        m_window->draw(drawable, transform);
    }

    [[nodiscard]]
    Vec2 getFocus() const
    {
        return -m_viewport_handler.state.offset;
    }

    [[nodiscard]]
    float getZoom() const
    {
        return m_viewport_handler.state.zoom;
    }

private:
    IVec2             m_render_size       = {};
    ViewportHandler   m_viewport_handler;
    sf::RenderWindow* m_window            = nullptr;

    void setWindow(sf::RenderWindow& window)
    {
        m_window = &window;
        auto const window_size = static_cast<Vec2>(m_window->getSize());
        m_viewport_handler.state.setCenter(window_size * 0.5f);
    }

    friend class WindowContextHandler;
};
}
