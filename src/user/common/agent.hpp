#pragma once
#include "user/common/neat/common_configuration.hpp"
#include "user/common/physic/solver.hpp"
#include "user/training/training_state.hpp"


struct Agent
{
    pbd::Solver system;

    using Vec2Real = sf::Vector2<nt::conf::RealType>;

    Agent() = default;

    void initialize(pbd::RealType compliance)
    {
        siv::Ref<pbd::Object> last_segment{};
        for (uint32_t i{0}; i < conf::sim::segments_count; ++i) {
            auto segment = system.createObject();
            segment->particles.emplace_back(0.0f, 0.0f);
            segment->particles.emplace_back(conf::sim::segment_size, 0.0f);
            segment->computeProperties();
            segment->position = pbd::Vec2D{conf::sim::world_size * 0.5f + Vec2{0.0f, (0.5f + float(i)) * conf::sim::segment_size}};
            segment->angle = Math::ConstantF32::Pi * 0.5f;

            // If this is not the first segment connect it with the previous one
            if (last_segment) {
                system.createObjectPinConstraint({last_segment, 1}, {segment, 0}, compliance);
            } else {
                auto c = system.createDragConstraint(segment, segment->getWorldPosition(0), compliance);
                auto const target = segment->getWorldPosition(0);
                c->target = target;
            }

            last_segment = segment;
        }
    }

    void update(pbd::RealType dt)
    {
        system.update(dt);
    }

    [[nodiscard]]
    Vec2Real getPosition(uint32_t i) const
    {
        return Vec2Real{system.objects[i].position};
    }

    [[nodiscard]]
    Vec2Real getBasePosition() const
    {
        return Vec2Real{system.objects[0].getWorldPosition(0)};
    }

    [[nodiscard]]
    Vec2Real getTipPosition() const
    {
        return Vec2Real{system.objects[conf::sim::segments_count - 1].getWorldPosition(1)};
    }

    [[nodiscard]]
    Vec2Real getDirection(uint32_t i) const
    {
        auto const angle = to<nt::conf::RealType>(system.objects[i].angle);
        return {cos(angle), sin(angle)};
    }

    [[nodiscard]]
    Vec2Real getVelocity(uint32_t i) const
    {
        return Vec2Real{system.objects[i].velocity};
    }

    [[nodiscard]]
    nt::conf::RealType getAngularVec(uint32_t i) const
    {
        return to<nt::conf::RealType>(system.objects[i].angular_velocity);
    }

    void applyDisturbance(pbd::Vec2D d)
    {
        system.objects[1].applyPositionCorrection({d.x, d.y}, {conf::sim::segment_size * 0.5, 0.0});
    }
};
