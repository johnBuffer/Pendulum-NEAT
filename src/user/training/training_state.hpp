#pragma once
#include <cstdint>
#include "user/common/physic/configuration.hpp"
#include "user/common/configuration.hpp"


struct TrainingState
{
    struct IterationConfiguration
    {
        pbd::RealType solver_friction   = 0.003;
        pbd::RealType solver_gravity    = 1.0;
        pbd::RealType solver_compliance = 0.0000;
        pbd::RealType max_speed         = 800.0;
        pbd::RealType max_accel         = 10000.0;

        uint32_t task_sub_steps   = 1;
        uint32_t solver_sub_steps = 8;
    };

    uint32_t      iteration             = 0;
    uint32_t      iteration_exploration = 0;
    pbd::RealType iteration_best_score  = 0.0f;

    IterationConfiguration configuration;

    bool demo = false;

    void addIteration()
    {
        ++iteration;
    }

    void endDemo()
    {
        demo = false;
    }

    void newExploration()
    {
        iteration            = 0;
        iteration_best_score = 0.0f;
        ++iteration_exploration;
    }
};