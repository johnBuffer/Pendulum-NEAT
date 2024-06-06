#pragma once
#include <algorithm>
#include "genome.hpp"
#include "network.hpp"


namespace nt
{
struct NetworkGenerator
{
    std::vector<uint32_t> idx_to_order;

    NetworkGenerator() = default;

    Network generate(nt::Genome& genome)
    {
        idx_to_order.resize(genome.info.getNodeCount());
        Network network;
        network.initialize(genome.info, static_cast<uint32_t>(genome.connections.size()));

        auto const order = getOrder(genome);
        for (uint32_t i{0}; i < order.size(); ++i) {
            idx_to_order[order[i]] = i;
        }

        // Create nodes and connections
        uint32_t node_idx{0};
        uint32_t connection_idx{0};
        for (uint32_t o : order) {
            // Initialize node
            auto const& node = genome.nodes[o];
            network.setNode(node_idx, node.activation, node.bias, genome.graph.nodes[o].getOutConnectionCount());
            network.setNodeDepth(node_idx, node.depth);
            // Create its connections
            for (auto const& c : genome.connections) {
                if (c.from == o) {
                    uint32_t const target = idx_to_order[c.to];
                    // Target node should be processed after this one
                    assert(target > node_idx);
                    network.setConnection(connection_idx, target, c.weight);
                    ++connection_idx;
                }
            }
            ++node_idx;
        }

        // Update network's max depth
        network.max_depth = network.getNode(node_idx - 1).depth;

        return network;
    }

    [[nodiscard]]
    std::vector<uint32_t> getOrder(nt::Genome& genome) const
    {
        genome.computeDepth();
        std::vector<uint32_t> order(genome.nodes.size());
        for (uint32_t i{0}; i < genome.nodes.size(); ++i) {
            order[i] = i;
        }

        // Only sort past the inputs to ensure inputs order (even if it shouldn't change anything)
        std::sort(order.begin() + genome.info.inputs, order.end(), [&genome](uint32_t a, uint32_t b) {
            return genome.nodes[a].depth < genome.nodes[b].depth;
        });

        return order;
    }
};
}