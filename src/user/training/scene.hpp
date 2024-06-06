#pragma once
#include <functional>
#include "user/common/agent.hpp"
#include "user/common/neat/network_generator.hpp"
#include "user/common/physic/configuration.hpp"
#include "user/common/disturbances.hpp"

#include "user/training/task.hpp"
#include "user/training/training_state.hpp"
#include "user/training/agent_info.hpp"


namespace training
{

struct Scene : public training::Task
{
    pez::core::ID agent_id         = pez::core::EntityID::INVALID_ID;
    pez::core::ID push_sequence_id = pez::core::EntityID::INVALID_ID;

    pbd::RealType freeze_time              = 2.0;
    pbd::RealType current_time             = 0.0;
    pbd::RealType current_velocity         = 0.0;

    TrainingState::IterationConfiguration configuration;

    bool enable_ai = true;

    nt::Network network;
    Agent       agent;

    // Disturbances
    bool          enable_disturbance       = false;
    pbd::RealType current_disturbance_time = 0.0;
    uint32_t      current_disturbance      = 0;
    pbd::RealType disturbance_freeze_time  = 0.0;

    // Score function
    pbd::RealType out_sum  = 0.0;
    pbd::RealType dist_sum = 0.0;
    pbd::RealType last_out = 0.0;

    using ScoreFunction = std::function<pbd::RealType(pbd::RealType, pbd::RealType, pbd::RealType)>;
    ScoreFunction score_function = nullptr;

    explicit
    Scene(pez::core::EntityID id_, pez::core::ID agent_id_, pez::core::ID sequence_id_)
        : Task{id_}
        , agent_id{agent_id_}
        , push_sequence_id{sequence_id_}
    {}

    void initialize()
    {
        // Reset variables
        current_time             = 0.0;
        current_velocity         = 0.0;
        current_disturbance_time = 0.0;
        current_disturbance      = 0;
        out_sum                  = 0.0;
        dist_sum                 = 0.0;
        last_out                 = 0.0;

        // Load state configuration
        auto const& state = pez::core::getSingleton<TrainingState>();
        configuration = state.configuration;

        // Reset agent
        agent = {};
        agent.initialize(configuration.solver_compliance);
        // Load solver configuration
        agent.system.gravity   = {0.0f, configuration.solver_gravity};
        agent.system.friction  = configuration.solver_friction;
        agent.system.sub_steps = configuration.solver_sub_steps;

        auto& agent_info = getAgentInfo();
        // Reset score
        agent_info.score = 0.0f;
        // Update the network
        network = agent_info.generateNetwork();
        enable_ai = true;
    }

    void update(pbd::RealType dt) override
    {
        auto const sub_dt = dt / static_cast<nt::conf::RealType>(configuration.task_sub_steps);
        for (uint32_t i{configuration.task_sub_steps}; i--;) {

            if (current_time >= freeze_time) {
                // Execute NN
                updateAI(sub_dt);
                // Update physics
                agent.update(sub_dt);
            }

            // Update disturbance only when outside freeze time
            if (enable_disturbance) {
                if (current_time >= (freeze_time + disturbance_freeze_time)) {
                    current_disturbance_time += sub_dt;
                    updateDisturbances(dt);
                }
            }

            current_time += sub_dt;
        }
    }

    void updateAI(pbd::RealType dt)
    {
        pbd::RealType const   pos_x     = (agent.getBasePosition().x - conf::sim::world_size.x * 0.5f) / (conf::sim::slider_length * 0.5f);
        Agent::Vec2Real const dir_1     = agent.getDirection(0);
        pbd::RealType const   ang_vel_1 = agent.getAngularVec(0);
        Agent::Vec2Real const dir_2     = agent.getDirection(1);
        pbd::RealType const   ang_vel_2 = agent.getAngularVec(1);
        pbd::RealType const   dot_1_2   = MathVec2::dot(dir_1, dir_2);

        if (enable_ai) {
            if (conf::net::control_type == conf::ControlType::Acceleration) {
                network.execute({
                                        pos_x,
                                        current_velocity / configuration.max_speed,
                                        dir_1.x,
                                        dir_1.y,
                                        ang_vel_1 * dt,
                                        dir_2.x,
                                        dir_2.y,
                                        ang_vel_2 * dt,
                                        dot_1_2
                                });
                update_velocity(network.output[0] * configuration.max_accel * dt);
            } else {
                network.execute({
                                        pos_x,
                                        dir_1.x,
                                        dir_1.y,
                                        ang_vel_1 * dt,
                                        dir_2.x,
                                        dir_2.y,
                                        ang_vel_2 * dt,
                                        dot_1_2
                                });
                current_velocity = network.output[0] * configuration.max_speed;
            }
            updateCartPosition(dt);
        }

        pbd::RealType const delta = std::abs(network.output[0] - last_out);
        pbd::RealType const pos_y = agent.getTipPosition().y;

        last_out = network.output[0];
        out_sum  += delta;
        dist_sum += std::abs(network.output[0]);

        if (score_function) {
            pbd::RealType const margin    = 0.1;
            pbd::RealType const height    = (float(conf::sim::segments_count) - margin) * conf::sim::segment_size;
            pbd::RealType const threshold = conf::sim::world_size.y * 0.5f - height;
            if (pos_y < threshold) {
                getAgentInfo().score += dt * score_function(pos_x, out_sum, dist_sum);
            }
        }
    }

    void update_velocity(pbd::RealType accel)
    {
        current_velocity += accel;
        if (current_velocity > configuration.max_speed) {
            current_velocity = configuration.max_speed;
        } else if (current_velocity < -configuration.max_speed) {
            current_velocity = -configuration.max_speed;
        }
    }

    void updateCartPosition(pbd::RealType dt)
    {
        auto& cart = agent.system.drag_constraints[0];
        cart.target.x += current_velocity * dt;

        // Handle limits
        float const min_pos = conf::sim::world_size.x * 0.5f - conf::sim::slider_length * 0.5f;
        float const max_pos = conf::sim::world_size.x * 0.5f + conf::sim::slider_length * 0.5f;

        if (cart.target.x < min_pos) {
            cart.target.x = min_pos;
            current_velocity = 0.0f;
        }
        if (cart.target.x > max_pos) {
            cart.target.x = max_pos;
            current_velocity = 0.0f;
        }
    }

    void updateDisturbances(pbd::RealType dt)
    {
        Disturbances::Push const& push = getCurrentPush();
        if (isActive(push)) {
            agent.applyDisturbance({push.force * dt, 0.0});
            if (isOver(push)) {
                ++current_disturbance;
                current_disturbance_time = 0.0f;
            }
        }
    }

    [[nodiscard]]
    Disturbances::Push const& getCurrentPush() const
    {
        return pez::core::get<Disturbances>(0).pushes[current_disturbance];
    }

    [[nodiscard]]
    bool isActive(Disturbances::Push const& push) const
    {
        return current_disturbance_time > push.delay;
    }

    [[nodiscard]]
    bool isOver(Disturbances::Push const& push) const
    {
        return (current_disturbance_time - push.delay) > push.duration;
    }

    [[nodiscard]]
    bool done() const override
    {
        return false;
    }

    AgentInfo& getAgentInfo()
    {
        return pez::core::get<AgentInfo>(agent_id);
    }
};

}
