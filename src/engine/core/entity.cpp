#include "entity.hpp"

namespace pez::core
{

Entity::Entity(EntityID id_)
    : id(id_)
{}

void Entity::requestRemove()
{
    need_remove = true;
}

bool Entity::isRemoved() const
{
    return need_remove;
}


}
