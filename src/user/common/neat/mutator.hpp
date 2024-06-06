#pragma once
#include "engine/common/number_generator.hpp"

#include "genome.hpp"
#include "user/common/configuration.hpp"


namespace nt
{
struct Mutator
{
    /// Mutates a genome using the probabilities defined in conf::mut
    static void mutateGenome(nt::Genome& genome) {
        for (uint32_t i{0}; i < ::conf::mut::mut_count; ++i) {
            if (RNGf::proba(0.25f)) {
                if (RNGf::proba(0.5f)) {
                    mutateBiases(genome);
                } else {
                    mutateWeights(genome);
                }
            }
        }
        if (RNGf::proba(::conf::mut::new_node_proba) && ::conf::mut::max_hidden_nodes > genome.info.hidden) {
            newNode(genome);
        }

        if (RNGf::proba(::conf::mut::new_conn_proba)) {
            newConnection(genome);
        }
    }

    static void mutateBiases(nt::Genome& genome)
    {
        Genome::Node& n = pickRandom(genome.nodes);
        if (RNGf::proba(::conf::mut::new_value_proba)) {
            n.bias = RNGf::getFullRange(::conf::mut::weight_range);
        } else {
            if (RNGf::proba(0.25f)) {
                n.bias += RNGf::getFullRange(::conf::mut::weight_range);
            } else {
                n.bias += ::conf::mut::weight_small_range * RNGf::getFullRange(::conf::mut::weight_range);
            }
        }
    }

    static void mutateWeights(nt::Genome& genome)
    {
        // Nothing to do if no connections
        if (genome.connections.empty()) {
            return;
        }

        Genome::Connection& c = pickRandom(genome.connections);
        if (RNGf::proba(::conf::mut::new_value_proba)) {
            c.weight = RNGf::getFullRange(::conf::mut::weight_range);
        } else {
            if (RNGf::proba(0.75f)) {
                c.weight += ::conf::mut::weight_small_range * RNGf::getFullRange(::conf::mut::weight_range);
            } else {
                c.weight += RNGf::getFullRange(::conf::mut::weight_range);
            }

        }
    }

    static void newNode(nt::Genome& genome)
    {
        // Nothing to do if no connections
        if (genome.connections.empty()) {
            return;
        }

        uint32_t const connection_idx = getRandIndex(genome.connections.size());
        genome.splitConnection(connection_idx);
    }

    static void newConnection(nt::Genome& genome)
    {
        // Pick first random node, input + hidden
        uint32_t const count_1 = genome.info.inputs + genome.info.hidden;
        uint32_t       idx_1   = getRandIndex(count_1);
        // If the picked node is an output, offset it by the number of outputs to land on hidden
        if (idx_1 >= genome.info.inputs && idx_1 < (genome.info.inputs + genome.info.outputs)) {
            idx_1 += genome.info.outputs;
        }
        // Pick second random node, hidden + output
        uint32_t const count_2 = genome.info.hidden + genome.info.outputs;
        // Skip inputs
        uint32_t       idx_2   = getRandIndex(count_2) + genome.info.inputs;

        assert(!genome.isOutput(idx_1));
        assert(!genome.isInput(idx_2));

        // Create the new connection
        if (!genome.tryCreateConnection(idx_1, idx_2, RNGf::getFullRange(::conf::mut::weight_range))) {
            //std::cout << "Cannot create connection " << idx_1 << " -> " << idx_2 << std::endl;
        }
    }

    static uint32_t getRandIndex(uint64_t max_value)
    {
        auto const max_value_f = static_cast<float>(max_value);
        return static_cast<uint32_t>(RNGf::getUnder(max_value_f));
    }

    template<typename TDataType>
    static TDataType& pickRandom(std::vector<TDataType>& container)
    {
        uint32_t const idx = getRandIndex(container.size());
        return container[idx];
    }
};
}
