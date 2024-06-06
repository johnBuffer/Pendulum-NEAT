#pragma once
#include "./vertex.hpp"
#include "./matrix.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "engine/common/math.hpp"

#include <vector>


namespace pbd
{

struct Object
{
    sf::Vector2<RealType> position      = {0.0, 0.0};
    sf::Vector2<RealType> position_last = {0.0, 0.0};
    RealType angle      = 0.0;
    RealType angle_last = 0.0;
    RealType density    = 1.0;

    sf::Vector2<RealType>  velocity;
    RealType angular_velocity = 0.0f;

    sf::Vector2<RealType> forces = {};

    sf::Vector2<RealType>  center_of_mass = {0.0, 0.0};
    RealType inv_mass           = 1.0;
    RealType inv_inertia_tensor = 1.0;

    std::vector<Vec2D> particles;

    sf::Color color = sf::Color::White;

    /// Computes inv_mass and inertia_tensor
    void computeProperties()
    {
        // Compute center of mass
        sf::Vector2<RealType>  pos_sum = {0.0, 0.0};
        RealType mass    = 0.0;
        for (auto const& p : particles) {
            pos_sum += p;
            mass    += 1.0;
        }
        center_of_mass = pos_sum / mass;
        inv_mass       = 1.0 / (density * mass);

        // Compute inertia_tensor
        RealType inertia_tensor = 0.0;
        for (auto const& p : particles) {
            inertia_tensor += (1.0 + MathVec2::length2(p - center_of_mass)) * density;
        }
        inv_inertia_tensor = 1.0 / inertia_tensor;
    }

    /**
     *
     * @param r Position of correction relative to center of mass
     * @param n Direction of correction
     * @return
     */
    [[nodiscard]]
    RealType getGeneralizedInvMass(Vec2D r, Vec2D n) const
    {
        RealType const cross_product = MathVec2::cross(r, n);
        return inv_mass + cross_product * inv_inertia_tensor * cross_product;
    }

    void update(RealType dt)
    {
        // Linear update
        position_last = position;
        velocity = velocity + dt * forces * inv_mass;
        position = position + velocity * dt;
        // Angular update
        angle_last = angle;
        // Not sure about this at all
        angle = angle + angular_velocity * dt;
    }

    void updateVelocities(RealType dt, RealType friction)
    {
        velocity         = (position - position_last) / dt * (1.0 - friction);
        angular_velocity = (angle - angle_last)       / dt * (1.0 - friction);
    }

    void applyPositionCorrection(Vec2D p, Vec2D r)
    {
        position += p * inv_mass;
        angle    += MathVec2::cross(r, p) * inv_inertia_tensor;
    }

    void applyRotationCorrection(float a)
    {
        angle += a * inv_inertia_tensor;
    }

    [[nodiscard]]
    Vec2D getWorldPosition(Vec2D obj_coord) const
    {
        glm::mat<4, 4, RealType> transform(1.0);
        transform = glm::translate(transform, {position.x, position.y, 0.0});
        transform = glm::rotate(transform, angle, {0.0, 0.0, 1.0});
        transform = glm::translate(transform, {-center_of_mass.x, -center_of_mass.y, 0.0});
        glm::vec<4, RealType> const result = transform * glm::vec4{obj_coord.x, obj_coord.y, 0.0, 1.0};
        return {result.x, result.y};
    }

    [[nodiscard]]
    Vec2D getWorldPosition(uint32_t idx) const
    {
        return getWorldPosition(particles[idx]);
    }

    [[nodiscard]]
    Vec2D getObjectPosition(Vec2D world_coord) const
    {
        glm::mat<4, 4, RealType> transform(1.0);
        transform = glm::translate(transform, {center_of_mass.x, center_of_mass.y, 0.0});
        transform = glm::rotate(transform, -angle, {0.0, 0.0, 1.0});
        transform = glm::translate(transform, {-position.x, -position.y, 0.0});
        glm::vec<4, RealType> const result = transform * glm::vec4{world_coord.x, world_coord.y, 0.0, 1.0};
        return {result.x, result.y};
    }
};

}
