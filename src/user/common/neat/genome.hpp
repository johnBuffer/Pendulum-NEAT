#pragma once
#include <vector>

#include "engine/common/binary_io.hpp"

#include "dag.hpp"
#include "network.hpp"
#include "common_configuration.hpp"


namespace nt
{
/// Blueprint a network
struct Genome
{
public: // Internal structs
    /// Represents a node of the network
    struct Node
    {
        conf::RealType      bias       = 0.0f;
        Activation activation = Activation::Sigm;
        uint32_t   depth      = 0;
    };

    /// Represents a connection between two nodes
    struct Connection
    {
        uint32_t from   = 0;
        uint32_t to     = 0;
        conf::RealType    weight = 0.0f;
    };

public: // Attributes
    /// Info
    nt::Network::Info       info;
    /// The nodes of the network
    std::vector<Node>       nodes;
    /// The connections
    std::vector<Connection> connections;
    /// A graph to create valid connections
    DAG                     graph;

public: // Methods
    Genome() = default;

    explicit
    Genome(uint32_t inputs, uint32_t outputs)
        : info{inputs, outputs}
    {
        // Create inputs
        for (uint32_t i{info.inputs}; i--;) {
            createNode(Activation::None, false);
        }
        // Create outputs
        for (uint32_t i{info.outputs}; i--;) {
            createNode(Activation::Tanh, false);
        }
    }

    uint32_t createNode(Activation activation, bool hidden = true)
    {
        nodes.emplace_back();
        nodes.back().activation = activation;
        nodes.back().bias       = 0.0f;

        graph.createNode();
        // Update info if needed
        if (hidden) {
            ++info.hidden;
        }
        // Return index of new node
        return static_cast<uint32_t>(nodes.size() - 1);
    }

    bool tryCreateConnection(uint32_t from, uint32_t to, conf::RealType weight)
    {
        if (graph.createConnection(from, to)) {
            connections.push_back({from, to, weight});
            return true;
        }
        return false;
    }

    void createConnection(uint32_t from, uint32_t to, conf::RealType weight)
    {
        graph.createConnection(from, to);
        connections.push_back({from, to, weight});
    }

    void splitConnection(uint32_t i)
    {
        if (i >= connections.size()) {
            std::cout << "Invalid connection " << i << std::endl;
        }

        Connection const& c      = connections[i];
        uint32_t const    from   = c.from;
        uint32_t const    to     = c.to;
        conf::RealType const       weight = c.weight;
        removeConnection(i);

        uint32_t const node_idx{createNode(Activation::Relu)};
        createConnection(from, node_idx, weight);
        createConnection(node_idx, to, 1.0f);
    }

    void removeConnection(uint32_t i)
    {
        graph.removeConnection(connections[i].from, connections[i].to);
        std::swap(connections[i], connections.back());
        connections.pop_back();
    }

    /// Returns nodes indexes sorted topologically
    [[nodiscard]]
    std::vector<uint32_t> getOrder() const
    {
        std::vector<uint32_t> order(nodes.size());
        for (uint32_t i{0}; i < nodes.size(); ++i) {
            order[i] = i;
        }

        std::sort(order.begin(), order.end(), [this](uint32_t a, uint32_t b) {
            return nodes[a].depth < nodes[b].depth;
        });

        return order;
    }

    /// Computes the depth of each node in the DAG
    void computeDepth()
    {
        auto const node_count = static_cast<uint32_t>(nodes.size());

        // Compute order
        uint32_t max_depth = 0;
        graph.computeDepth();
        for (uint32_t i{0}; i < node_count; ++i) {
            nodes[i].depth = graph.nodes[i].depth;
            max_depth = std::max(nodes[i].depth, max_depth);
        }

        // Set outputs to the last "layer"
        uint32_t const output_depth = std::max(max_depth, 1u);
        for (uint32_t i{0}; i < info.outputs; ++i) {
            nodes[info.inputs + i].depth = output_depth;
        }
    }

    [[nodiscard]]
    bool isInput(uint32_t i) const
    {
        return i < info.inputs;
    }

    [[nodiscard]]
    bool isOutput(uint32_t i) const
    {
        return (i >= info.inputs) && (i < info.inputs + info.outputs);
    }

    void writeToFile(std::string const& filename) const
    {
        BinaryWriter writer(filename);
        writer.write(info);
        for (auto const& n : nodes) {
            writer.write(n);
        }
        writer.write(connections.size());
        for (auto const& c : connections) {
            writer.write(c);
        }
    }

    void loadFromFile(std::string const& filename)
    {
        // Create the reader
        BinaryReader reader(filename);
        if (!reader.isValid()) {
            std::cout << "Cannot open file \"" << filename << "\"" << std::endl;
        }

        // Clear graph
        graph.nodes.clear();

        // Load info
        reader.readInto(info);
        nodes.resize(info.getNodeCount());

        // Load nodes
        for (auto& n : nodes) {
            reader.readInto(n);
            graph.createNode();
        }

        // Load connections
        auto const connection_count = reader.read<size_t>();
        for (size_t i{0}; i < connection_count; ++i) {
            auto const c = reader.read<Connection>();
            createConnection(c.from, c.to, c.weight);
        }

        std::cout << "\"" << filename << "\" loaded." << std::endl;
    }
};
}