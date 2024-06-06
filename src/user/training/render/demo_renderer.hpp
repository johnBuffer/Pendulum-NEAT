#pragma once
#include "engine/engine.hpp"

#include "user/common/render/agent_renderer.hpp"
#include "user/common/render/card.hpp"
#include "user/common/render/empty_card.hpp"
#include "user/common/render/graph_widget.hpp"
#include "user/common/render/network_renderer.hpp"
#include "user/common/render/multi_graph_widget.hpp"
#include "user/common/render/tracer.hpp"
#include "user/common/disturbances.hpp"

#include "user/training/scene.hpp"

#include "./iteration_state.hpp"
#include "./scene_state.hpp"


namespace training
{

struct DemoRenderer
{
    // Constants
    float const     world_padding     = 25.0f;
    float const     outline           = 5.0f;
    sf::Color const accent_color      = {231, 111, 81};
    sf::Color const small_graph_color = {244, 162, 97};
    float const     slider_margin     = 100.0f;
    float const     padding           = 20.0f;

    // Text
    sf::Font& font;

    // Scene element
    Card background;
    Card background_outline;
    AgentRenderer agent_renderer;
    EmptyCard slider;
    bool best_only = true;

    // HUD
    // --- Graphs ---
    GraphWidget output_plot;
    GraphWidget visu_angle_1;
    GraphWidget visu_angle_2;
    MultiGraphWidget visu_angle_vs;

    // --- State ---
    IterationState  iteration_state;
    SceneState      scene_state;
    NetworkRenderer network_renderer;

    Tracer tracer_tip;
    Tracer tracer_mid;

    // Disturbances
    sf::Texture arrow_texture;

    explicit
    DemoRenderer()
        : font{pez::resources::getFont("font")}
        , background{conf::sim::world_size + Vec2{50.0f, 50.0f}, 25.0f, {50, 50, 50}}
        , background_outline{conf::sim::world_size + 2.0f * Vec2{world_padding + outline, world_padding + outline}, 25.0f + outline, accent_color}
        , slider{{conf::sim::slider_length + slider_margin, 6.0f}, 3.0f, sf::Color::White}
        , output_plot{{}}
        , visu_angle_1{{}}
        , visu_angle_2{{}}
        , visu_angle_vs{{}}
        , iteration_state({})
        , scene_state{{}}
    {
        auto const& state = pez::core::getSingleton<TrainingState>();

        // Background
        background.position         = {-25.0f, -25.0f};
        background_outline.position = background.position - Vec2{outline, outline};

        // Slider
        slider.setThickness(2.0f);
        slider.position = conf::sim::world_size * 0.5f - Vec2{(conf::sim::slider_length + slider_margin) * 0.5f, slider.corner_radius};

        // Agent
        agent_renderer.cart.offset.y = slider.thickness + slider.corner_radius;

        // Network renderer
        network_renderer.setFont(font);

        // Plots
        float const graph_height = conf::net::control_type == conf::ControlType::Acceleration ? 403.0f : 369.0f;
        // --- output ---
        if (conf::net::control_type == conf::ControlType::Acceleration) {
            output_plot.setTitle("Output (acceleration)");
        } else {
            output_plot.setTitle("Output (velocity)");
        }
        output_plot.setSize({2.25f * graph_height, graph_height});
        output_plot.chart.extremes = {0.0f, 0.0f};
        output_plot.setColor({233, 196, 106});
        output_plot.chart.values.setMaxValuesCount(200);
        output_plot.height_round = 0.1f;
        output_plot.label_callback = [](uint32_t i) {
            return toString(to<float>(i) * 0.016f, 1);
        };
        if (conf::net::control_type == conf::ControlType::Acceleration) {
            output_plot.current_value_callback = [](float x) {
                return toString(x * 0.01f, 1) + " m/s^2";
            };
        } else {
            output_plot.current_value_callback = [](float x) {
                return toString(x * 0.01f, 1) + " m/s";
            };
        }

        float const small_font_scale = 0.8f;
        // --- angle 1 ---
        visu_angle_1.setTitle("Angle base");
        visu_angle_1.setColor({138, 177, 125});
        visu_angle_1.label_callback = [](uint32_t i) {
            return toString(to<float>(i) * 0.016f, 1);
        };
        visu_angle_1.current_value_callback = [](float x) {
            return toString(x, 1) + " deg";
        };
        visu_angle_1.font_scale = small_font_scale;

        // --- angle 2 ---
        visu_angle_2.setTitle("Angle mid");
        visu_angle_2.setColor({244, 162, 97});
        visu_angle_2.label_callback = [](uint32_t i) {
            return toString(to<float>(i) * 0.016f, 1);
        };
        visu_angle_2.current_value_callback = [](float x) {
            return toString(x, 1) + " deg";
        };
        visu_angle_2.font_scale = small_font_scale;

        // --- multi angle ---
        visu_angle_vs.setTitle("Angle base vs mid");
        visu_angle_vs.setColor({231, 111, 81});
        visu_angle_vs.font_scale = small_font_scale;

        // Tracers
        float const speed  = 1.2f;
        float const radius = 8.0f;
        float const radius_end = 0.2f;
        tracer_tip.width_start = radius;
        tracer_mid.width_start = radius;
        tracer_tip.width_end = radius_end;
        tracer_mid.width_end = radius_end;
        tracer_tip.interpolation = Interpolation::Linear;
        tracer_mid.interpolation = Interpolation::Linear;
        tracer_tip.width_speed = speed;
        tracer_mid.width_speed = speed;
        tracer_tip.setColor(agent_renderer.getJointColor(AgentRenderer::Mode::Solid, 2));
        tracer_mid.setColor(agent_renderer.getJointColor(AgentRenderer::Mode::Solid, 1));

        arrow_texture.loadFromFile("res/arrow.png");
    }

    void render(pez::render::Context& context)
    {
        auto const& scene_best = pez::core::get<Scene>(0);

        background_outline.render(context);
        background.render(context);

        slider.render(context);
        drawHorizontalTicks(context);

        // Agents
        // --- Draw other ---
        uint32_t const agent_count  = best_only ? 1 : conf::sel::population_size;
        for (uint32_t i{1}; i < agent_count; ++i) {
            auto const& scene = pez::core::get<Scene>(i);
            agent_renderer.renderAgent(context, scene.agent, AgentRenderer::Mode::Ghost);
        }
        // --- Draw best ---
        agent_renderer.renderAgent(context, scene_best.agent, AgentRenderer::Mode::Solid);

        // Disturbance
        Disturbances::Push const& push = scene_best.getCurrentPush();
        if (scene_best.isActive(push)) {
            auto const force = to<float>(push.force);
            Vec2 const size{8, 5};
            sf::RectangleShape arrow{size};
            arrow.setOrigin(size.x, size.y * 0.5f);
            arrow.setTexture(&arrow_texture);
            arrow.setPosition(Vec2{scene_best.agent.system.objects[1].getWorldPosition(1)});
            arrow.setScale(force, force);
            auto const alpha = to<uint8_t>(std::max(0.0f, 255.0f));
            arrow.setFillColor({255, 255, 255, uint8_t(alpha)});
            context.draw(arrow);
        }

        // Neural network
        network_renderer.update();
        network_renderer.render(context);

        // Graphs
        output_plot.addValue(to<float>(scene_best.network.output[0] * scene_best.configuration.max_accel * 0.01));
        output_plot.render(context);

        auto const& system = scene_best.agent.system;
        auto const angle_1 = to<float>(270.0 - Math::radToDeg(system.objects[0].angle));
        visu_angle_1.addValue(angle_1);
        visu_angle_1.render(context);

        auto const angle_2 = to<float>(Math::radToDeg(system.objects[0].angle) - Math::radToDeg(system.objects[1].angle));
        visu_angle_2.addValue(angle_2);
        visu_angle_2.render(context);

        visu_angle_vs.addPoint({angle_1, angle_2});
        visu_angle_vs.render(context);

        iteration_state.render(context);

        scene_state.setAiState(scene_best.enable_ai);
        scene_state.setDisturbanceState(scene_best.enable_disturbance);
        scene_state.setTime(to<float>(scene_best.current_time - scene_best.freeze_time));
        scene_state.render(context);
    }

    void updateNetwork(nt::Network const& network)
    {
        Vec2 const render_size = pez::render::getRenderSize();
        float const zoom = pez::render::getZoom();
        network_renderer.initialize(network);
        std::cout << network_renderer.size.y << std::endl;

        float const hud_width = network_renderer.size.x + output_plot.size.x + padding;

        float const start_x = (render_size.x - hud_width) * 0.5f;
        float const hud_start_y = background_outline.size.y * zoom + 1.0f * padding;
        float const hud_height  = render_size.y - hud_start_y;
        float const start_y = hud_start_y + (hud_height - network_renderer.size.y) * 0.5f;

        network_renderer.setPosition({start_x, start_y});
        output_plot.setPosition({start_x + network_renderer.size.x + padding, start_y});

        // Small graphs
        float const small_graph_size = (render_size.x - background_outline.size.x * zoom) * 0.5f - 2.0f * padding;
        visu_angle_1.setSize({small_graph_size, small_graph_size});
        visu_angle_1.setPosition({render_size.x - small_graph_size - padding, padding});

        visu_angle_2.setSize({small_graph_size, small_graph_size});
        visu_angle_2.setPosition({render_size.x - small_graph_size - padding, padding + small_graph_size + padding});

        visu_angle_vs.setSize({small_graph_size, small_graph_size});
        visu_angle_vs.setPosition({render_size.x - small_graph_size - padding, padding + 2.0f * (small_graph_size + padding)});

        scene_state.setSize({small_graph_size, 208.0f});
        scene_state.setPosition({padding, padding});

        iteration_state.setSize({small_graph_size, 360.0f});
        iteration_state.setPosition({padding, scene_state.size.y + 2.0f * padding});
    }

    void drawHorizontalTicks(pez::render::Context& context)
    {
        float const   tick_width = 10.0f;
        int32_t const tick_count = 30;

        float const mid_length = 10.0f;
        float const y_offset   = conf::sim::world_size.y * 0.5f + 40.0f;
        sf::VertexArray mid_line{sf::Lines, 4 * tick_count + 2};

        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(60);
        float const scale = 0.4f;

        uint32_t idx{0};
        for (int32_t i{-tick_count}; i <= tick_count; ++i) {
            float const x = conf::sim::world_size.x * 0.5f + i * tick_width;
            float const y_scale = (i % 10 == 0) ? 2.0f : 1.0f;
            mid_line[2 * idx + 0].position = Vec2{x, y_offset - mid_length * 0.5f * y_scale};
            mid_line[2 * idx + 1].position = Vec2{x, y_offset + mid_length * 0.5f * y_scale};

            if (i % 10 == 0) {
                if (i) {
                    text.setScale(0.5f * scale, 0.5f * scale);
                } else {
                    text.setScale(1.0f * scale, 1.0f * scale);
                }

                text.setString(toString(i * tick_width, 0));
                text.setPosition(x - text.getGlobalBounds().width * 0.5f - 3.5f * text.getScale().x, y_offset + mid_length + 2.0f);
                context.draw(text);

                mid_line[2 * idx + 0].color = accent_color;
                mid_line[2 * idx + 1].color = accent_color;
            } else {
                mid_line[2 * idx + 0].color = sf::Color::White;
                mid_line[2 * idx + 1].color = sf::Color::White;
            }
            ++idx;
        }

        context.draw(mid_line);
    }

    void reset_graphs()
    {
        output_plot.clear();
        visu_angle_vs.clear();
    }
};

}
