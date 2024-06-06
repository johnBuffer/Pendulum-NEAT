#include "demo.hpp"
#include "user/training/render/renderer.hpp"


void training::Demo::initialize()
{
    enable_disturbance = false;
    initialized        = true;

    // Initialize tasks
    pez::core::parallelForeach<training::Scene>([&](training::Scene& s) {
        // Use reference push sequence
        s.push_sequence_id   = 0;
        s.enable_disturbance = enable_disturbance;
        s.initialize();
        s.freeze_time = 1.0f;
    });

    auto& renderer = pez::core::getRenderer<Renderer>();
    auto const& scene = pez::core::get<Scene>(0);

    renderer.demo_renderer.updateNetwork(scene.network);
    renderer.demo_renderer.reset_graphs();
    renderer.demo_renderer.iteration_state.updateState(0.003f);
    renderer.demo_renderer.tracer_tip.clear();
    renderer.demo_renderer.tracer_mid.clear();
}
