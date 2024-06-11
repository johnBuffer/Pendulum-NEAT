#pragma once
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>


struct DAG
{
    struct Node
    {
        uint32_t              incoming = 0;
        uint32_t              depth    = 0;
        std::vector<uint32_t> out;

        [[nodiscard]]
        uint32_t getOutConnectionCount() const
        {
            return static_cast<uint32_t>(out.size());
        }
    };

    std::vector<Node> nodes;

    void createNode()
    {
        nodes.emplace_back();
    }

    bool createConnection(uint32_t from, uint32_t to)
    {
        // Ensure both nodes exist
        if (!isValid(from)) {
            //std::cout << "Cannot create connection " << from << " -> " << to << ": " << from << " is not valid" << std::endl;
            return false;
        }
        if (!isValid(to)) {
            //std::cout << "Cannot create connection " << from << " -> " << to << ": " << to << " is not valid" << std::endl;
            return false;
        }
        // Ensure there is no cycle
        if (from == to) {
            //std::cout << "Cannot connect " << from << " to itself" << std::endl;
            return false;
        }

        if (isAncestor(to, from)) {
            //std::cout << "Cannot create connection " << from << " -> " << to << ": " << to << " is an ancestor of " << from << std::endl;
            return false;
        }
        // Ensure the connection doesn't already exist
        if (isParent(from, to)) {
            //std::cout << "Cannot create connection " << from << " -> " << to << ": already connected" << std::endl;
            return false;
        }
        // Add the connection in the parent node
        nodes[from].out.push_back(to);
        // Increase the incoming connections count of the child node
        nodes[to].incoming++;
        return true;
    }

    [[nodiscard]]
    bool isValid(uint32_t i) const
    {
        return i < nodes.size();
    }

    /// Checks if @p node_1 has @p node_2 in its children list
    [[nodiscard]]
    bool isParent(uint32_t node_1, uint32_t node_2) const
    {
        auto& out = nodes[node_1].out;
        return std::any_of(out.begin(), out.end(), [&](uint32_t o) { return (o == node_2); });
    }

    /// Checks if @p node_1 is an ancestor of @p node_2
    [[nodiscard]]
    bool isAncestor(uint32_t node_1, uint32_t node_2) const
    {
        auto& out = nodes[node_1].out;
        return isParent(node_1, node_2) || std::any_of(out.begin(), out.end(), [&](uint32_t o) {
            return isAncestor(o, node_2);
        });
    }

    /// Computes the depth of each node
    void computeDepth()
    {
        auto const node_count = nodes.size();
        // Nodes with no incoming edge
        std::vector<uint32_t> start_nodes;
        // Current incoming edge state
        std::vector<uint32_t> incoming;
        incoming.reserve(node_count);

        // Initialize incoming state
        for (auto const& n : nodes) {
            incoming.push_back(n.incoming);
        }

        // Initialize the set of nodes with no incoming edge
        uint32_t i{0};
        for (auto& n : nodes) {
            if (n.incoming == 0) {
                n.depth = 0;
                start_nodes.push_back(i);
            }
            ++i;
        }

        // Perform the sort
        while (!start_nodes.empty()) {
            // Extract a node from the starting set
            uint32_t const idx = start_nodes.back();
            start_nodes.pop_back();

            // Remove incoming connection for all children of this node
            Node const& n = nodes[idx];
            for (auto const o : n.out) {
                incoming[o]--;
                Node& connected = nodes[o];
                connected.depth = std::max(connected.depth, n.depth + 1);
                // If a children has no incoming edge anymore, add it to the starting set
                if (incoming[o] == 0) {
                    start_nodes.push_back(o);
                }
            }
        }
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

    void removeConnection(uint32_t from, uint32_t to)
    {
        auto&      connections = nodes[from].out;
        auto const count       = static_cast<uint32_t>(connections.size());
        uint32_t found = 0;
        for (uint32_t i{0}; i < count - found;) {
            if (connections[i] == to) {
                std::swap(connections[i], connections.back());
                connections.pop_back();
                --nodes[to].incoming;
                ++found;
            } else {
                ++i;
            }
        }

        if (!found) {
            std::cout << "[WARNING] Connection " << from << " -> " << to << " not found" << std::endl;
        }
    }
};
