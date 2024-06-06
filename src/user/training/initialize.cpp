#include "./initialize.hpp"
#include "engine/engine.hpp"

#include "user/training/stadium.hpp"
#include "user/training/agent_info.hpp"
#include "user/training/training_state.hpp"
#include "user/training/scene.hpp"
#include "user/training/demo.hpp"
#include "user/common/disturbances.hpp"

#include "user/training/render/renderer.hpp"


namespace training
{

void registerSystems()
{
    pez::core::registerSingleton<TrainingState>();

    pez::core::registerDataEntity<Disturbances>();
    pez::core::registerDataEntity<AgentInfo>();
    pez::core::registerDataEntity<Scene>();

    pez::core::registerProcessor<Stadium>();
    pez::core::registerProcessor<Demo>();

    pez::core::registerRenderer<Renderer>();
}

void loadResources()
{
    pez::resources::registerFont("res/font.ttf", "font");
    pez::resources::registerTexture("res/wheel_2.png", "wheel");
}

}
