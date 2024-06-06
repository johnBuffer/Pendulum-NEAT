#pragma once
#include "engine/common/index_vector.hpp"
#include "engine/common/utils.hpp"

#include "./configuration.hpp"
#include "./constraints/constraint.hpp"
#include "./object.hpp"
#include "./complex.hpp"

#include "./constraints/drag_constraint.hpp"
#include "./constraints/object_pin.hpp"

namespace pbd
{

struct Solver
{
    siv::IndexVector<Object> objects;

    siv::IndexVector<DragConstraint>      drag_constraints;
    siv::IndexVector<ObjectPinConstraint> object_pins;

    Vec2D         gravity  = {0.0, 1000.0};
    pbd::RealType friction = 0.0;

    uint32_t sub_steps{2};

    void update(RealType dt)
    {
        uint32_t const pos_iter{1};
        RealType const sub_dt{dt / to<RealType>(sub_steps)};

        for (uint32_t i{sub_steps}; i--;) {
            for (auto &obj: objects) {
                obj.forces = gravity / obj.inv_mass;
                obj.update(sub_dt);
            }

            resetConstraints();
            for (uint32_t k{pos_iter}; k--;) {
                solveConstraints(sub_dt);
            }

            for (auto& obj: objects) {
                obj.updateVelocities(sub_dt, friction);
            }
        }
    }

    siv::Ref<DragConstraint> createDragConstraint(siv::Ref<Object> obj, Vec2D target, RealType compliance)
    {
        auto const constraint_id = drag_constraints.emplace_back();
        auto& constraint = drag_constraints[constraint_id];
        constraint.create(obj, target);
        constraint.constraint.compliance = compliance;
        return drag_constraints.createRef(constraint_id);
    }

    void remove(siv::Ref<DragConstraint> const& c)
    {
        drag_constraints.erase(c);
    }

    // Maybe replace with anchor
    siv::Ref<ObjectPinConstraint> createObjectPinConstraint(Anchor anchor_1, Anchor anchor_2, RealType compliance)
    {
        auto const constraint_id = object_pins.emplace_back();
        auto& constraint = object_pins[constraint_id];
        constraint.create(anchor_1, anchor_2);
        constraint.constraint.compliance = compliance;
        return object_pins.createRef(constraint_id);
    }

    void remove(siv::Ref<ObjectPinConstraint> const& c)
    {
        object_pins.erase(c);
    }

    siv::Ref<Object> createObject()
    {
        siv::ID const id = objects.emplace_back();
        return objects.createRef(id);
    }

    void resetConstraints()
    {
        for (auto& c: drag_constraints) {
            c.constraint.lambda = 0.0;
        }

        for (auto& c: object_pins) {
            c.constraint.lambda = 0.0;
        }
    }

    void solveConstraints(RealType dt)
    {
        for (auto& c: drag_constraints) {
            c.solve(dt);
        }

        for (auto& c: object_pins) {
            c.solve(dt);
        }
    }
};

}