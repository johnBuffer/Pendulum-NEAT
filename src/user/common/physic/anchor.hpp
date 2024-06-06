#pragma once
#include "engine/common/index_vector.hpp"
#include "./object.hpp"


namespace pbd
{
struct Anchor
{
    siv::Ref<Object> obj       = {};
    Vec2D            obj_coord = {};

    Anchor() = default;

    /** Creates an anchor
     *
     * @param object The object on which the anchor is placed
     * @param coord The coordinates in object space of the anchor
     */
    Anchor(siv::Ref<Object> object, Vec2D coord)
        : obj{object}
        , obj_coord{coord}
    {}

    /** Creates an anchor
     *
     * @param object The object on which the anchor is placed
     * @param particle_idx The idx of the particle on which the anchor will be placed
     */
    Anchor(siv::Ref<Object> object, uint32_t particle_idx)
        : obj{object}
        , obj_coord{object->particles[particle_idx]}
    {}
};
}