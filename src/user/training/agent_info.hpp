#pragma once
#include "engine/engine.hpp"
#include "engine/common/number_generator.hpp"

#include "user/common/configuration.hpp"
#include "user/common/neat/genome.hpp"
#include "user/common/neat/network_generator.hpp"

#include "user/common/physic/configuration.hpp"


struct AgentInfo : public pez::core::Entity
{
    /// Attributes
    pbd::RealType score = 0.0f;
    nt::Genome    genome;

    /// Methods
    AgentInfo() = default;

    explicit
    AgentInfo(pez::core::EntityID id_)
        : pez::core::Entity{id_}
    {
        resetGenome();
    }

    void onRemove() override
    {}

    [[nodiscard]]
    nt::Network generateNetwork()
    {
        return nt::NetworkGenerator().generate(genome);
    }

    void resetGenome()
    {
        genome = nt::Genome{conf::net::input_count, conf::net::output_count};
    }

    void createRandomFullConnections()
    {
        for (uint32_t i{0}; i < genome.info.inputs; ++i) {
            for (uint32_t k{0}; k < genome.info.outputs; ++k) {
                genome.createConnection(i, genome.info.inputs + k, RNGf::getFullRange(conf::mut::weight_range));
            }
        }
    }
};