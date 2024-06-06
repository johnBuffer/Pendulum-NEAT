#pragma once
#include "entity_id.hpp"
#include "engine/render/render_context.hpp"


namespace pez::core
{

struct Entity
{
    EntityID id;
    bool     need_remove    = false;
    bool     pre_remove_ack = false;

    Entity() = default;

    explicit
    Entity(EntityID id_);

    void requestRemove();

    virtual void onRemove() {};

    [[nodiscard]]
    bool isRemoved() const;
};

}
