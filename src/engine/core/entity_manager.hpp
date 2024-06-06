#pragma once

#include <iostream>

#include "system.hpp"
#include "entity.hpp"
#include "entity_container.hpp"

namespace pez::core
{

struct EntityManager
{
    using ProcessCallback  = void(*)(float);
    using RenderCallback   = void(*)(pez::render::Context&);
    using VoidCallback     = void(*)();
    using ValidityCallback = bool(*)(const EntityRef&);

    uint32_t                      class_count    = 0;
    std::vector<ProcessCallback>  update_callbacks;
    std::vector<RenderCallback>   render_callbacks;
    std::vector<VoidCallback>     pre_remove_callbacks;
    std::vector<VoidCallback>     remove_callbacks;
    std::vector<VoidCallback>     on_stop_callbacks;
    std::vector<VoidCallback>     clear_callbacks;
    std::vector<ValidityCallback> validity_callbacks;
    std::vector<VoidCallback>     clear_systems;

    template<typename T>
    void registerEntity()
    {
        if (EntityContainer<T>::isRegistered()) {
            std::cout << "WARNING: Entity already registered" << std::endl;
            return;
        }
        EntityContainer<T>::class_id = class_count++;
        pre_remove_callbacks.push_back(EntityContainer<T>::callPreRemoveCallbacks);
        remove_callbacks.push_back(EntityContainer<T>::removeObjects);
        clear_callbacks.push_back(EntityContainer<T>::clear);
        validity_callbacks.push_back(EntityContainer<T>::isValid);
    }

    template<typename T>
    void registerDataEntity()
    {
        if (EntityContainer<T>::isRegistered()) {
            std::cout << "WARNING: Entity already registered" << std::endl;
            return;
        }
        EntityContainer<T>::class_id = class_count++;
        clear_callbacks.push_back(EntityContainer<T>::clear);
        validity_callbacks.push_back(EntityContainer<T>::isValid);
    }

    template<typename T, typename... Arg>
    void registerProcessor(Arg&&... args)
    {
        if (System<T>::isRegistered()) {
            std::cout << "WARNING: Processor already registered" << std::endl;
            return;
        }
        static_assert(std::is_convertible<T*, IProcessor*>::value, "Provided class is not a Processor");
        System<T>::create(std::forward<Arg>(args)...);
        update_callbacks.push_back(Processor<T>::update);
        on_stop_callbacks.push_back(System<T>::stop);
        clear_systems.push_back(System<T>::clear);
    }

    template<typename T, typename ...Arg>
    void registerRenderer(Arg&&... args)
    {
        if (System<T>::isRegistered()) {
            std::cout << "WARNING: Renderer already registered" << std::endl;
            return;
        }
        static_assert(std::is_convertible<T*, IRenderer*>::value, "Provided class is not a Renderer");
        System<T>::create(std::forward<Arg>(args)...);
        render_callbacks.push_back(Renderer<T>::render);
        on_stop_callbacks.push_back(System<T>::stop);
        clear_systems.push_back(System<T>::clear);
    }

    template<typename T, typename... Arg>
    void registerSingleton(Arg&&... args)
    {
        if (Singleton<T>::isRegistered()) {
            std::cout << "WARNING: Singleton already registered" << std::endl;
            return;
        }
        Singleton<T>::create(std::forward<Arg>(args)...);
        clear_systems.push_back(Singleton<T>::clear);
    }

    void updateEntities(float dt)
    {
        for (const ProcessCallback& f : update_callbacks) {
            f(dt);
        }
    }

    void removeEntities()
    {
        // First call pre remove callbacks, nothing removed at this point
        for (const VoidCallback& f : pre_remove_callbacks) {
            f();
        }
        // Then actually remove the objects
        for (const VoidCallback& f : remove_callbacks) {
            f();
        }
    }

    void render(pez::render::Context& context)
    {
        for (const RenderCallback& f : render_callbacks) {
            f(context);
        }
    }

    void clearEntities()
    {
        for (const VoidCallback& f : clear_callbacks) {
            f();
        }
    }

    void stopSystems()
    {
        for (const VoidCallback& f : on_stop_callbacks) {
            f();
        }
    }

    void clearSystems()
    {
        for (const VoidCallback& f : clear_systems) {
            f();
        }
    }

    template<typename T, typename... Arg>
    EntityID create(const Arg&&... args)
    {
        return T::create(std::forward<Arg>(args)...);
    }

    template<typename TEntity>
    bool isValid(const EntityRef& ref)
    {
        return validity_callbacks[EntityContainer<TEntity>::class_id](ref);
    }
};

}
