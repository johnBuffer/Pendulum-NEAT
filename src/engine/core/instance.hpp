#pragma once
#include "engine/render/render_context.hpp"
#include "engine/resources/resource_manager.hpp"
#include "entity_manager.hpp"
#include "system.hpp"


namespace pez::core
{

struct EngineInstance
{
    EntityManager                    m_entity_manager;
    pez::render::Context*            m_render_context   = nullptr;
    pez::resources::ResourceManager* m_resource_manager = nullptr;

    // Time related attributes
    float    time  = 0.0f;
    uint64_t tick  = 0;
    bool     pause = false;

    EngineInstance();

    void update(float dt);
    void quit();
    void render();
};

struct GlobalInstance
{
    // Singleton instance
    static core::EngineInstance* instance;
};

}
