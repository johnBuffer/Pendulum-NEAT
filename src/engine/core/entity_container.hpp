#pragma once
#include <limits>
#include "engine/common/index_vector.hpp"
#include "entity_id.hpp"


namespace pez::core
{

template<typename TEntity>
struct EntityContainer
{
    static constexpr uint32_t        invalid_id = std::numeric_limits<uint32_t>::max();
    static uint32_t                  class_id;
    static siv::IndexVector<TEntity> data;

    EntityContainer() = default;

    static bool isRegistered()
    {
        return class_id != invalid_id;
    }

    static void preRemove()
    {
        for (TEntity& entity : data) {
            entity.onRemove();
        }
    }

    template<typename... Arg>
    static ID create(Arg&&... args)
    {
        const core::EntityID next_id = {class_id, static_cast<uint32_t>(data.getNextID())};
        return static_cast<ID>(data.emplace_back(next_id, std::forward<Arg>(args)...));
    }

    static void callPreRemoveCallbacks() {
        for (TEntity& obj : data) {
            if (obj.need_remove && !obj.pre_remove_ack) {
                obj.pre_remove_ack = true;
                obj.onRemove();
            }
        }
    }

    static void removeObjects() {
        data.remove_if([](const TEntity& obj) {
            return obj.need_remove && obj.pre_remove_ack;
        });
    }

    static void clear() {
        for (TEntity& entity : data) {
            entity.onRemove();
        }
        data.remove_if([](const TEntity& obj) {
            return true;
        });
    }

    static bool isValid(const EntityRef& ref)
    {
        return ref.id.class_id == class_id && data.isValid(ref.id.instance_id, ref.validity);
    }
};

template<typename T>
uint32_t EntityContainer<T>::class_id = invalid_id;

template<typename T>
siv::IndexVector<T> EntityContainer<T>::data = {};

template<typename T>
T& getEntity(siv::ID id)
{
    return EntityContainer<T>::data[id];
}

template<typename T>
uint32_t getCount()
{
    return static_cast<uint32_t>(EntityContainer<T>::data.size());
}

}
