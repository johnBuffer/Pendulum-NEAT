#pragma once
#include <cstdint>

#include "engine/core/instance.hpp"
#include "engine/core/timer.hpp"
#include "engine/render/render.hpp"
#include "engine/resources/resources.hpp"

#include "engine/common/thread_pool/thread_pool.hpp"


namespace pez::core
{

void     createSystems(uint32_t thread_count = 0);
void     quit();
void     update(float dt);
void     render(sf::Color clear_color = sf::Color::Black);
uint64_t getTick();
float    getTime();
void     setPause(bool pause);
void     togglePause();
bool     isRunning();

/**
 * Creates the singletons that are tied to the engine
 *
 * @param thread_count The number of threads to use in the thread pool.
 *                     0 means automatic thread count based on available CPU cores.
 */
void createDefaultSingletons(uint32_t thread_count);

template<typename T>
uint32_t getClassID()
{
    return EntityContainer<T>::class_id;
}

template<typename T>
static siv::IndexVector<T>& getData()
{
    return core::EntityContainer<T>::data;
}

template<typename T, typename... Arg>
static ID create(Arg&&... args)
{
    return core::EntityContainer<T>::create(std::forward<Arg>(args)...);
}

template<typename T, typename... Arg>
static void createMultiple(uint32_t count, Arg&&... args)
{
    for (uint32_t i{count}; i--;) {
        core::EntityContainer<T>::create(std::forward<Arg>(args)...);
    }
}

template<typename T>
static T& get(siv::ID id)
{
    return core::EntityContainer<T>::data[id];
}

template<typename T>
siv::Ref<T> getRef(siv::ID id)
{
    return core::EntityContainer<T>::data.createRef(id);
}

template<typename T>
T& getProcessor()
{
    static_assert(std::is_convertible<T*, IProcessor*>::value, "Provided class is not an IProcessor");
    return *core::System<T>::instance;
}

template<typename T>
T& getRenderer()
{
    static_assert(std::is_convertible<T*, IRenderer*>::value, "Provided class is not an IRenderer");
    return *core::System<T>::instance;
}

template<typename T>
T& getSingleton()
{
    return *core::Singleton<T>::instance;
}

template<typename T, typename... Arg>
siv::Ref<T> createGetRef(Arg&&... args)
{
    const siv::ID id = core::EntityContainer<T>::create(std::forward<Arg>(args)...);
    return core::EntityContainer<T>::data.createRef(id);
}

template<typename T>
bool isValid(const core::EntityRef& ref)
{
    return GlobalInstance::instance->m_entity_manager.isValid<T>(ref);
}

bool isValidRef(const core::EntityRef& ref);

// This name isn't nice because of civ::Ref
template<typename T>
core::EntityRef createEntityRef(core::EntityID id)
{
    return {id.class_id, id.instance_id, core::EntityContainer<T>::data.getValidityID(id.instance_id)};
}

template<typename T>
core::EntityRef createEntityRef(core::ID id)
{
    return {EntityContainer<T>::class_id, id, core::EntityContainer<T>::data.getValidityID(id)};
}

template<typename T>
T& get(const core::EntityRef& ref)
{
    return get<T>(ref.id.instance_id);
}

template<typename T>
bool isInstanceOf(const core::EntityRef& ref)
{
    return core::EntityContainer<T>::class_id == ref.id.class_id;
}

template<typename T>
bool isInstanceOf(const core::EntityID& id)
{
    return core::EntityContainer<T>::class_id == id.class_id;
}

template<typename T>
void registerEntity()
{
    core::GlobalInstance::instance->m_entity_manager.registerEntity<T>();
}

template<typename T>
void registerDataEntity()
{
    core::GlobalInstance::instance->m_entity_manager.registerDataEntity<T>();
}

template<typename T, typename... TArg>
void registerProcessor(TArg&&... args)
{
    core::GlobalInstance::instance->m_entity_manager.registerProcessor<T>(std::forward<TArg>(args)...);
}

template<typename T, typename... TArg>
static void registerRenderer(TArg&&... args)
{
    core::GlobalInstance::instance->m_entity_manager.registerRenderer<T>(std::forward<TArg>(args)...);
}

template<typename T, typename... TArg>
static void registerSingleton(TArg&&... args)
{
    core::GlobalInstance::instance->m_entity_manager.registerSingleton<T>(std::forward<TArg>(args)...);
}

template<typename T>
void remove(ID id)
{
    EntityContainer<T>::data.erase(id);
}

template<typename T, typename TCallback>
void foreach(TCallback&& callback) {
    static_assert(std::is_convertible<T*, Entity*>::value, "Can only iterate on Entity derived objects");
    std::vector<T>& data = core::EntityContainer<T>::data.getData();
    const uint64_t count = core::EntityContainer<T>::data.size();
    for (uint64_t i{0}; i<count; ++i) {
        if (!data[i].isRemoved()) {
            callback(data[i]);
        }
    }
}

template<typename T, typename TCallback>
void foreachAbort(TCallback&& callback) {
    static_assert(std::is_convertible<T*, Entity*>::value, "Can only iterate on Entity derived objects");
    std::vector<T>& data = core::EntityContainer<T>::data.getData();
    const uint64_t count = core::EntityContainer<T>::data.size();
    for (uint64_t i{0}; i<count; ++i) {
        if (!data[i].isRemoved()) {
            if (callback(data[i])) {
                return;
            }
        }
    }
}

template<typename T, typename TCallback>
void parallelForeach(TCallback&& callback) {
    static_assert(std::is_convertible<T*, Entity*>::value, "Can only iterate on Entity derived objects");
    std::vector<T>& data  = core::EntityContainer<T>::data.getData();
    auto const      count = static_cast<uint32_t>(core::EntityContainer<T>::data.size());

    auto& tp = pez::core::getSingleton<tp::ThreadPool>();
    tp.dispatch(count, [&data, callback](uint32_t start, uint32_t end) {
        for (uint32_t i{start}; i < end; ++i) {
            if (!data[i].isRemoved()) {
                callback(data[i]);
            }
        }
    });
}

}
