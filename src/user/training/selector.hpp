#pragma once
#include <iostream>
#include <vector>

#include "engine/common/number_generator.hpp"


struct Selector
{
    struct Entry
    {
        uint32_t index       = 0;
        float    score       = 0.0f;
        // This is the sum score used for selection
        float    wheel_score = 0.0f;
    };

    std::vector<Entry> entries;

    void clear()
    {
        entries.clear();
    }

    void addEntry(const Entry& entry)
    {
        entries.push_back(entry);
    }

    void addEntry(uint32_t index, float score)
    {
        entries.push_back({index, score, 0.0f});
    }

    void normalizeEntries()
    {
        float sum = 0.0f;
        for (const auto& e : entries) {
            sum += e.score;
        }

        // If all scores are 0, we use virtual uniform scores
        float normalized_sum = 0.0f;
        if (sum == 0.0f) {
            const float virtual_score = 1.0f / static_cast<float>(entries.size());
            for (auto& e : entries) {
                normalized_sum += virtual_score;
                e.wheel_score   = normalized_sum;
            }
        } else {
            for (auto& e : entries) {
                normalized_sum += e.score / sum;
                e.wheel_score   = normalized_sum;
            }
        }
    }

    [[nodiscard]]
    uint32_t pick() const
    {
        if (entries.empty()) {
            std::cout << "No entries, returning 0." << std::endl;
            return 0;
        }

        const float score_threshold = RNGf::getUnder(1.0f);
        for (const auto& e : entries) {
            if (e.wheel_score > score_threshold) {
                return e.index;
            }
        }
        return entries.back().index;
    }
};
