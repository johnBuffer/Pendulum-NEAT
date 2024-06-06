#pragma once
#include <memory>
#include "entity_container.hpp"
#include "entity.hpp"
#include "engine/render/render_context.hpp"


namespace pez::core
{

struct ISystem
{
    virtual void stop() {}
};

struct IProcessor : public ISystem
{
    virtual void update(float dt) {}
};

struct IRenderer : public ISystem
{
    virtual void render(pez::render::Context& context) {}
};

/// System
template<typename T>
struct System
{
    static std::unique_ptr<T> instance;

    template<typename... Arg>
    static void create(Arg&&... args)
    {
        instance = std::make_unique<T>(std::forward<Arg>(args)...);
    }

    static T& get()
    {
        return *instance;
    }

    static void stop()
    {
        instance->stop();
    }

    static void clear()
    {
        instance = nullptr;
    }

    static bool isRegistered()
    {
        return instance != nullptr;
    }
};

template<typename T>
std::unique_ptr<T> System<T>::instance = nullptr;

/// Singleton (no update function)
template<typename T>
struct Singleton
{
    static std::unique_ptr<T> instance;

    template<typename... Arg>
    static void create(Arg&&... args)
    {
        instance = std::make_unique<T>(std::forward<Arg>(args)...);
    }

    static T& get()
    {
        return *instance;
    }

    static void clear()
    {
        instance = nullptr;
    }

    static bool isRegistered()
    {
        return instance != nullptr;
    }
};

template<typename T>
std::unique_ptr<T> Singleton<T>::instance = nullptr;


template<typename T>
struct Processor
{
    static void update(float dt)
    {
        System<T>::instance->update(dt);
    }
};


template<typename T>
struct Renderer
{
    static void render(pez::render::Context& context)
    {
        System<T>::instance->render(context);
    }
};

}

