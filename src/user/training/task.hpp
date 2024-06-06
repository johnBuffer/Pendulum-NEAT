#pragma once
#include "engine/engine.hpp"
#include "user/common/neat/common_configuration.hpp"


namespace training
{

struct Task : public pez::core::Entity
{
    Task() = default;

    explicit
    Task(pez::core::EntityID id_)
        : pez::core::Entity{id_}
    {}

    /// Prepare for the task before its execution
    virtual void initialize() = 0;

    /// The update to perform at each timestep of the task's execution
    virtual void update(nt::conf::RealType dt) = 0;

    /// Compute the resulting score
    virtual void computeScore() {};

    /// Check if the the task is finished
    [[nodiscard]]
    virtual bool done() const { return false; }
};

}
