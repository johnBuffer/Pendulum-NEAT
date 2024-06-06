#pragma once
#include <vector>

#include "activation.hpp"
#include "common_configuration.hpp"


namespace nt
{
struct Network
{

public: // Internal structs
    struct Info
    {
        uint32_t inputs  = 0;
        uint32_t outputs = 0;
        uint32_t hidden  = 0;

        Info() = default;

        Info(uint32_t inputs_, uint32_t outputs_)
            : inputs{inputs_}
            , outputs{outputs_}
            , hidden{0}
        {}

        [[nodiscard]]
        uint32_t getNodeCount() const
        {
            return inputs + hidden + outputs;
        }
    };

    struct Node
    {
        ActivationPtr activation       = ActivationFunction::none;
        conf::RealType      sum              = 0.0;
        conf::RealType      bias             = 0.0;
        uint32_t      connection_count = 0;
        uint32_t      depth            = 0;

        [[nodiscard]]
        conf::RealType getValue() const
        {
            return activation(sum + bias);
        }
    };

    struct Connection
    {
        uint32_t to      = 0;
        conf::RealType weight = 0.0;
        conf::RealType value  = 0.0;
    };

    union Slot
    {
        // By default, slot is initialized as a Node, just to allow resizing
        Node       node       = {};
        Connection connection;

        // Constructor required to use resize on vector
        Slot()
            : node{}
        {}
    };

public: // Attributes
    std::vector<Slot>     slots;
    std::vector<conf::RealType> output;

    Info     info;
    uint32_t max_depth        = 0;
    uint32_t connection_count = 0;

public: // Methods
    Network() = default;

    /// Initializes the slots vector
    void initialize(Info const& info_, uint32_t connection_count_)
    {
        info             = info_;
        connection_count = connection_count_;

        slots.resize(info.getNodeCount() + connection_count);
        output.resize(info.outputs);
    }

    void setNode(uint32_t i, Activation activation, conf::RealType bias, uint32_t connection_count_)
    {
        getNode(i).activation       = ActivationFunction::getFunction(activation);
        getNode(i).bias             = bias;
        getNode(i).connection_count = connection_count_;
    }

    void setNodeDepth(uint32_t i, uint32_t depth)
    {
        slots[i].node.depth = depth;
    }

    void setConnection(uint32_t i, uint32_t to, conf::RealType weight)
    {
        Connection& connection = getConnection(i);
        connection.to     = to;
        connection.weight = weight;
    }

    [[nodiscard]]
    Connection const& getConnection(uint32_t i) const
    {
        return slots[info.getNodeCount() + i].connection;
    }

    Connection& getConnection(uint32_t i)
    {
        return slots[info.getNodeCount() + i].connection;
    }

    Node& getNode(uint32_t i)
    {
        return slots[i].node;
    }

    [[nodiscard]]
    Node const& getNode(uint32_t i) const
    {
        return slots[i].node;
    }

    Node& getOutput(uint32_t i)
    {
        return slots[info.inputs + info.hidden + i].node;
    }

    bool execute(std::vector<conf::RealType> const& input)
    {
        // Check compatibility
        if (input.size() != info.inputs) {
            std::cout << "Input size mismatch, aborting" << std::endl;
            return false;
        }

        // Reset nodes
        foreachNode([](Node& n, uint32_t) {
            n.sum = 0.0;
        });

        // Initialize input
        for (uint32_t i{0}; i < info.inputs; ++i) {
            slots[i].node.sum = input[i];
        }

        // Execute network
        uint32_t current_connection = 0;
        uint32_t const node_count = info.getNodeCount();
        for (uint32_t i{0}; i < node_count; ++i) {
            Node const&    node  = slots[i].node;
            conf::RealType const value = node.getValue();
            for (uint32_t o{0}; o < node.connection_count; ++o) {
                Connection& c = getConnection(current_connection++);
                c.value = value * c.weight;
                getNode(c.to).sum += c.value;
            }
        }

        // Update output
        for (uint32_t i{0}; i < info.outputs; ++i) {
            output[i] = getOutput(i).getValue();
        }

        return true;
    }

    [[nodiscard]]
    std::vector<conf::RealType> const& getResult() const
    {
        return output;
    }

    template<typename TCallback>
    void foreachNode(TCallback&& callback)
    {
        uint32_t const node_count = info.getNodeCount();
        for (uint32_t i{0}; i < node_count; ++i) {
            callback(slots[i].node, i);
        }
    }

    template<typename TCallback>
    void foreachNode(TCallback&& callback) const
    {
        uint32_t const node_count = info.getNodeCount();
        for (uint32_t i{0}; i < node_count; ++i) {
            callback(slots[i].node, i);
        }
    }

    template<typename TCallback>
    void foreachConnection(TCallback&& callback) const
    {
        for (uint32_t i{0}; i < connection_count; ++i) {
            callback(getConnection(i), i);
        }
    }

    /// Returns the depth of the network
    [[nodiscard]]
    uint32_t getDepth() const
    {
        return max_depth;
    }
};
}
