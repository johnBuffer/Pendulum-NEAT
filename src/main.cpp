#include "engine/window/window_context_handler.hpp"

#include "user/common/configuration.hpp"

#include "user/training/render/renderer.hpp"
#include "user/training/initialize.hpp"
#include "user/training/stadium.hpp"
#include "user/training/demo.hpp"


int main()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    settings.depthBits = conf::win::bit_depth;
    pez::render::WindowContextHandler app("Pendulum - Training", sf::Vector2u(conf::win::window_width, conf::win::window_height), settings, sf::Style::Fullscreen);
    training::loadResources();
    training::registerSystems();

    auto& renderer = pez::core::getRenderer<training::Renderer>();
    float const zoom = 1.87f;
    pez::render::setZoom(zoom);

    float const viewport_world_size_y = conf::win::window_height / zoom;
    float const target_x = conf::sim::world_size.x * 0.5f;
    float const offset_y = 0.5f * viewport_world_size_y - renderer.world_padding - renderer.outline - renderer.card_margin / zoom;
    pez::render::setFocus({target_x, offset_y});

    app.getEventManager().addKeyPressedCallback(sf::Keyboard::S, [&](sfev::CstEv) {
        app.toggleUnlimitedFramerate();
    });

    app.getEventManager().addKeyPressedCallback(sf::Keyboard::Space, [&](sfev::CstEv) {
        pez::core::getProcessor<Stadium>().bypass_score_threshold = true;
    });

    app.getEventManager().addKeyPressedCallback(sf::Keyboard::BackSpace, [&](sfev::CstEv) {
        pez::core::getProcessor<Stadium>().bypass_score_threshold = true;
        pez::core::getSingleton<TrainingState>().configuration.solver_friction *= 0.95f;
    });

    app.getEventManager().addKeyPressedCallback(sf::Keyboard::A, [&](sfev::CstEv) {
        training::Demo::toggleAI();
    });

    app.getEventManager().addKeyPressedCallback(sf::Keyboard::B, [&](sfev::CstEv) {
        renderer.toggleBestOnly();
    });

    app.getEventManager().addKeyPressedCallback(sf::Keyboard::P, [&](sfev::CstEv) {
        pez::core::getProcessor<training::Demo>().toggleDisturbances();
    });

    app.getEventManager().addKeyPressedCallback(sf::Keyboard::W, [&](sfev::CstEv) {
        try
        {
            pez::core::getProcessor<Stadium>().writeAllGenomes();
        } catch (std::exception const& e) {
            std::cout << "Couldn't save genomes: " << e.what() << std::endl;
        }
    });

    app.getEventManager().addKeyPressedCallback(sf::Keyboard::D, [&](sfev::CstEv) {
        app.disableFullSpeed();
        pez::core::getProcessor<training::Demo>().toggle();
    });

    constexpr uint32_t fps_cap = 60;
    const float dt = 1.0f / static_cast<float>(fps_cap);
    while (app.run()) {
        pez::core::update(dt);
        pez::core::render({80, 80, 80});
    }

    return 0;
}
