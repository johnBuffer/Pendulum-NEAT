#pragma once
#include "engine/common/utils.hpp"

#include "./selector.hpp"
#include "user/common/neat/mutator.hpp"
#include "user/training/agent_info.hpp"


struct Evolver
{
    using AgentInfoVector = std::vector<AgentInfo>;

    TrainingState& state;

    Selector selector;

    AgentInfoVector old_generation;
    AgentInfoVector new_generation;

    Evolver()
        : state{pez::core::getSingleton<TrainingState>()}
    {
        old_generation.resize(conf::sel::population_size);
        new_generation.resize(conf::sel::population_size);
    }

    void createNewGeneration()
    {
        fetchOldPopulation();
        new_generation.clear();
        selector.clear();

        std::sort(old_generation.begin(), old_generation.end(), [](const AgentInfo& a1, const AgentInfo& a2) {
            return a1.score > a2.score;
        });

        state.iteration_best_score = old_generation[0].score;
        std::cout << "[" << state.iteration << "] Iteration best: " << state.iteration_best_score << std::endl;

        // Keep elite
        const auto elite_count = to<uint32_t>(conf::sel::elite_ratio * to<float>(conf::sel::population_size));
        for (uint32_t i{0}; i < elite_count; ++i) {
            new_generation.push_back(old_generation[i]);
        }

        {
            uint32_t i{0};
            for (const auto& g: old_generation) {
                selector.addEntry(i, to<float>(g.score));
                ++i;
            }
        }
        selector.normalizeEntries();

        // Create new genomes
        while (to<uint32_t>(new_generation.size()) < conf::sel::population_size) {
            const uint32_t genome_idx = selector.pick();
            new_generation.push_back(old_generation[genome_idx]);
            auto& new_genome = new_generation.back();
            // Mutate genome
            nt::Mutator::mutateGenome(new_genome.genome);
        }

        updatePopulation();
    }

    void fetchOldPopulation()
    {
        uint32_t i{0};
        pez::core::foreach<AgentInfo>([&](AgentInfo& a) {
            old_generation[i] = a;
            ++i;
        });
    }

    void updatePopulation()
    {
        uint32_t i{0};
        pez::core::foreach<AgentInfo>([&](AgentInfo& a) {
            a = new_generation[i];
            ++i;
        });
    }
};