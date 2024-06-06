#pragma once
#include "engine/engine.hpp"
#include "engine/common/smooth/smooth_value.hpp"
#include "engine/common/chart/line_chart.hpp"

#include "user/common/render/utils.hpp"
#include "user/common/render/network_renderer.hpp"
#include "user/common/render/card.hpp"
#include "user/common/render/empty_card.hpp"
#include "user/common/render/graph_widget.hpp"
#include "user/common/render/bar_graph_widget.hpp"

#include "user/training/demo.hpp"
#include "user/training/render/time_state.hpp"
#include "user/training/render/demo_renderer.hpp"
#include "user/training/render/training_renderer.hpp"

#include "user/common/render/agent_renderer.hpp"


namespace training
{

    struct Renderer : public pez::core::IRenderer
    {
        float const     world_padding    = 25.0f;
        float const     outline          = 5.0f;
        float const     card_margin      = 20.0f;
        sf::Color const accent_color     = {231, 111, 81};
        sf::Color const hud_accent_color = {100, 170, 255};

        TrainingState& state;

        DemoRenderer     demo_renderer;
        TrainingRenderer training_renderer;

        explicit
        Renderer()
            : state{pez::core::getSingleton<TrainingState>()}
        {
            std::vector<std::string> network_input_labels;
            if (conf::net::control_type == conf::ControlType::Acceleration) {
                network_input_labels = {
                        "Position",
                        "Velocity",
                        "Direction_1 x",
                        "Direction_1 y",
                        "Angular_vel_1",
                        "Direction_2 x",
                        "Direction_2 y",
                        "Angular_vel_2",
                        "dot(dir_1, dir_2)"
                };
            } else {
                network_input_labels = {
                        "Position",
                        "Direction_1 x",
                        "Direction_1 y",
                        "Angular_vel_1",
                        "Direction_2 x",
                        "Direction_2 y",
                        "Angular_vel_2",
                        "dot(dir_1, dir_2)"
                };
            }

            training_renderer.network_renderer.labels = network_input_labels;
            demo_renderer.network_renderer.labels     = network_input_labels;
        }

        void render(pez::render::Context& context) override
        {
            if (state.demo) {
                demo_renderer.render(context);
            } else {
                training_renderer.render(context);
            }
        }

        void toggleBestOnly()
        {
            demo_renderer.best_only = !demo_renderer.best_only;
        }
    };

}