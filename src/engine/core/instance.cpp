#include "instance.hpp"

namespace pez::core
{

EngineInstance::EngineInstance()
{
    m_render_context   = new pez::render::Context();
    m_resource_manager = new pez::resources::ResourceManager();
}

void EngineInstance::quit()
{
    delete m_render_context;
    // Remove all entities
    m_entity_manager.clearEntities();
    // Stop all systems before removing
    m_entity_manager.stopSystems();
    m_entity_manager.clearSystems();
}

void EngineInstance::render()
{

}

void EngineInstance::update(float dt)
{
    if (pause) { return; }
    // Update all entities
    m_entity_manager.updateEntities(dt);
    // Remove entities that requested removal
    m_entity_manager.removeEntities();
    // Time update
    time += dt;
    tick++;
}

pez::core::EngineInstance* pez::core::GlobalInstance::instance = nullptr;

}
