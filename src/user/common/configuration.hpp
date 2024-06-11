#pragma once
#include "engine/common/vec.hpp"

namespace conf
{

enum class ControlType : uint8_t
{
    None         = 0,
    Velocity     = 1,
    Acceleration = 2
};

namespace win
{
    constexpr uint32_t window_width  = 2560;
    constexpr uint32_t window_height = 1440;
    constexpr uint32_t bit_depth = 24;

    //constexpr uint32_t window_width  = 1600;
    //constexpr uint32_t window_height = 900;
}

namespace net
{
    const ControlType control_type = ControlType::Velocity;

    const uint32_t input_count  = (control_type == ControlType::Acceleration) ? 9 : 8;
    const uint32_t output_count = 1;
}


namespace sim
{
    constexpr float    segment_size   = 100.0f;
    constexpr float    slider_length  = 500.0f;
    constexpr float    max_gravity    = 1000.0f;
    constexpr uint32_t segments_count = 2;
    const Vec2         world_size     = {slider_length + 2.2f * segments_count * segment_size,
                                         segments_count * segment_size * 2.25f};
}

namespace sel
{
    constexpr uint32_t population_size    = 1000;
    constexpr float    max_iteration_time = 60.0f;
    constexpr float    elite_ratio        = 0.35f;
}

namespace mut
{
    constexpr float new_node_proba = 0.05f;
    constexpr float new_conn_proba = 0.8f;

    constexpr float new_value_proba      = 0.2f;
    constexpr double weight_range        = 1.0;
    constexpr double weight_small_range  = 0.01;

    constexpr uint32_t mut_count = 4;
    constexpr uint32_t max_hidden_nodes = 30;
}

namespace exp
{
    constexpr uint32_t seed_offset        = 101;
    constexpr uint32_t best_save_period   = 10;
    constexpr uint32_t exploration_period = 1000;
}

}
