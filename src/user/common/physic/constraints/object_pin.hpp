#pragma once
#include "engine/common/index_vector.hpp"
#include "../object.hpp"
#include "../anchor.hpp"

#include "./constraint.hpp"

namespace pbd
{

struct ObjectPinConstraint
{
    Constraint constraint;

    Anchor anchor_1;
    Anchor anchor_2;

    void create(Anchor anchor_1_, Anchor anchor_2_)
    {
        anchor_1 = anchor_1_;
        anchor_2 = anchor_2_;
    }

    void solve(RealType dt)
    {
        Vec2D const anchor_1_world_position = anchor_1.obj->getWorldPosition(anchor_1.obj_coord);
        Vec2D const anchor_2_world_position = anchor_2.obj->getWorldPosition(anchor_2.obj_coord);
        Vec2D const r1 = anchor_1_world_position - anchor_1.obj->position;
        Vec2D const r2 = anchor_2_world_position - anchor_2.obj->position;

        Vec2D const    v = anchor_1_world_position - anchor_2_world_position;
        RealType const d = MathVec2::length(v);
        if (d == 0.0f) {
            return;
        }
        Vec2D const n = v / d;

        RealType const w1 = anchor_1.obj->getGeneralizedInvMass(r1, n);
        RealType const w2 = anchor_2.obj->getGeneralizedInvMass(r2, -n);
        RealType const a            = constraint.compliance / (dt * dt);
        RealType const delta_lambda = (d - a * constraint.lambda) / (w1 + w2 + a);
        constraint.lambda       += delta_lambda;

        Vec2D const p = delta_lambda * n;
        anchor_1.obj->applyPositionCorrection(-p, r1);
        anchor_2.obj->applyPositionCorrection( p, r2);

        constraint.force = constraint.lambda / (dt * dt);
    }
};

}
