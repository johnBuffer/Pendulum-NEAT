#pragma once
#include <cstdint>
#include <limits>


namespace pez::core
{

using ID = uint32_t;

struct EntityID
{
    static constexpr uint32_t INVALID_ID = std::numeric_limits<uint32_t>::max();

    ID class_id    = INVALID_ID;
    ID instance_id = INVALID_ID;

    EntityID() = default;
    EntityID(uint32_t class_id_, uint32_t instance_id_)
        : class_id(class_id_)
        , instance_id(instance_id_)
    {}

    [[nodiscard]]
    bool isValid() const
    {
        return (class_id != INVALID_ID) && (instance_id != INVALID_ID);
    }

    template<typename T>
    [[nodiscard]]
    bool isInstanceOf()
    {
        return class_id == T::class_id;
    }

    void reset()
    {
        class_id    = INVALID_ID;
        instance_id = INVALID_ID;
    }

    [[nodiscard]]
    bool operator==(const EntityID& other) const
    {
        return (class_id == other.class_id) && (instance_id == other.instance_id);
    }
};

struct EntityRef
{
    EntityID id;
    uint64_t validity;

    EntityRef()
        : validity{EntityID::INVALID_ID}
    {}

    EntityRef(EntityID id_, uint64_t validity_)
        : id(id_)
        , validity(validity_)
    {}

    EntityRef(uint32_t class_id, uint32_t instance_id, uint64_t validity_)
        : id{class_id, instance_id}
        , validity{validity_}
    {}

    [[nodiscard]]
    bool operator==(const EntityRef& other) const
    {
        return other.id == id && other.validity == validity;
    }

    [[nodiscard]]
    bool operator!=(const EntityRef& other) const
    {
        return !(*this == other);
    }
};

}
