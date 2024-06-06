#include "./render.hpp"

#include "../engine.hpp"


namespace pez::render
{
    pez::render::Context* getContext()
    {
        return pez::core::GlobalInstance::instance->m_render_context;
    }

    void setFocus(Vec2 focus)
    {
        pez::core::GlobalInstance::instance->m_render_context->setFocus(focus);
    }

    /// Move the focus by d pixels in the window coordinates system
    void moveFocusRelative(Vec2 d)
    {
        auto        render_context = pez::core::GlobalInstance::instance->m_render_context;
        Vec2 const  current_focus  = render_context->getFocus();
        float const current_zoom   = render_context->getZoom();

        setFocus(current_focus + d / current_zoom);
    }

    void setZoom(float zoom)
    {
        pez::core::GlobalInstance::instance->m_render_context->setZoom(zoom);
    }

    Vec2 getFocus()
    {
        return pez::core::GlobalInstance::instance->m_render_context->getFocus();
    }

    float getZoom()
    {
        return pez::core::GlobalInstance::instance->m_render_context->getZoom();
    }

    void clear(sf::Color color)
    {
        pez::core::GlobalInstance::instance->m_render_context->clear(color);
    }

    void draw(sf::Drawable const& drawable)
    {
        pez::core::GlobalInstance::instance->m_render_context->drawDirect(drawable);
    }

    void display()
    {
        pez::core::GlobalInstance::instance->m_render_context->display();
    }

    Vec2 getRenderSize()
    {
        IVec2 const render_size = pez::core::GlobalInstance::instance->m_render_context->m_size;
        return static_cast<Vec2>(render_size);
    }
}