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

#include "user/common/render/agent_renderer.hpp"


namespace training
{

struct TrainingRenderer
{
    float const     outline          = 5.0f;
    float const     card_margin      = 20.0f;
    sf::Color const accent_color     = {231, 111, 81};
    sf::Color const hud_accent_color = {100, 170, 255};
    Vec2 const      graph_size       = {800.0f, 326.0f};
    NetworkRenderer network_renderer;

    TrainingState& state;

    BarGraphWidget fitness;
    GraphWidget gravity_plot;
    GraphWidget friction_plot;

    TimeState time_state;

    explicit
    TrainingRenderer()
        : state{pez::core::getSingleton<TrainingState>()}
        , fitness{1.5f * graph_size}
        , gravity_plot{graph_size}
        , friction_plot{graph_size}
        , time_state{{42, 157, 143}}
    {
        time_state.setFont(pez::resources::getFont("font"));

        Vec2 const render_size = pez::render::getRenderSize();

        fitness.chart.setColor(hud_accent_color);
        fitness.chart.data.setMaxValuesCount(200);
        fitness.current_value.setCharacterSize(20);
        fitness.setPosition({(render_size.x - fitness.size.x) * 0.5f, 130.0f});
        fitness.setTitle("Score");
        fitness.setColor(hud_accent_color);
        fitness.current_value_callback = [](float x) {
            return toString(x, 5);
        };

        float const graph_start_x = (render_size.x - (2.0f * graph_size.x + card_margin)) * 0.5f;
        gravity_plot.chart.extremes = {0.0f, 0.0f};
        gravity_plot.setColor({233, 196, 106});
        gravity_plot.chart.values.setMaxValuesCount(200);
        gravity_plot.setPosition({graph_start_x, fitness.position.y + fitness.size.y + card_margin});
        gravity_plot.setTitle("Gravity");

        friction_plot.chart.extremes = {0.0f, 0.0f};
        friction_plot.setColor({231, 111, 81});
        friction_plot.chart.values.setMaxValuesCount(200);
        friction_plot.setPosition({graph_start_x + graph_size.x + card_margin, fitness.position.y + fitness.size.y + card_margin});
        friction_plot.setTitle("Friction");
        friction_plot.height_round = 0.001f;
        friction_plot.current_value_callback = [](float x) {
            return toString(x, 10);
        };

        network_renderer.setFont(pez::resources::getFont("font"));

        time_state.setPosition({card_margin, card_margin});
    }

    void render(pez::render::Context& context)
    {
        // Time state
        time_state.setIteration(state.iteration);
        time_state.render(context);

        fitness.render(context);

        gravity_plot.render(context);
        friction_plot.render(context);

        // Neural network
        if (!state.demo && state.iteration) {
            updateNetwork(pez::core::get<AgentInfo>(0).generateNetwork());
            network_renderer.render(context);
        }
    }

    void updateNetwork(nt::Network const& network)
    {
        network_renderer.initialize(network);
        network_renderer.setPosition({(pez::render::getRenderSize().x - network_renderer.size.x) * 0.5f,
                                      gravity_plot.position.y + gravity_plot.size.y + card_margin});
    }
};

}