#pragma once
#include "engine/engine.hpp"
#include "user/training/training_state.hpp"
#include "user/training/scene.hpp"


namespace training
{

struct Demo : public pez::core::IProcessor
{
    TrainingState&  state;
    tp::ThreadPool& thread_pool;

    bool  initialized = false;

    bool          enable_disturbance = false;
    pbd::RealType max_friction       = 0.0;

    Demo()
        : state{pez::core::getSingleton<TrainingState>()}
        , thread_pool{pez::core::getSingleton<tp::ThreadPool>()}
    {
        max_friction = state.configuration.solver_friction;
    }

    void initialize();

    void update(float dt) override
    {
        if (!state.demo) {
            return;
        }

        if (!initialized) {
            initialize();
        }

        updateAgents(dt);
    }

    static Scene& getBest()
    {
        return pez::core::get<Scene>(0);
    }

    static void toggleAI()
    {
        auto& best     = getBest();
        best.enable_ai = !best.enable_ai;
    }

    void toggleDisturbances()
    {
        enable_disturbance = !enable_disturbance;
        pez::core::foreach<training::Scene>([&](training::Scene& s) {
            s.enable_disturbance = enable_disturbance;
        });
    }

    void toggle()
    {
        setActive(!state.demo);
    }

    void endDemo()
    {
        initialized = false;
        auto& best = getBest();
        state.endDemo();
        std::cout << "Demo score: " << best.getAgentInfo().score << std::endl;

        // Restore training scene configuration
        pez::core::parallelForeach<training::Scene>([&](training::Scene& s) {
            s.push_sequence_id   = 1;
            s.freeze_time        = 0.0f;
            s.enable_disturbance = false;
        });
    }

    void setActive(bool b)
    {
        state.demo = b;
        if (b) {
            initialize();
        } else {
            endDemo();
        }
    }

    void updateAgents(float dt)
    {
        uint32_t const tasks_count = pez::core::getCount<training::Scene>();
        auto&          tasks       = pez::core::getData<training::Scene>().getData();

        thread_pool.dispatch(tasks_count, [&](uint32_t start, uint32_t end) {
            for (uint32_t i{start}; i < end; ++i) {
                if (!tasks[i].done()) {
                    tasks[i].update(dt);
                }
            }
        });
    }
};

}
