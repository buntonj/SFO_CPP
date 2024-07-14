// Fixtures for testing certain types of cost functions.
#pragma once
#include <gtest/gtest.h>

// include the cost function and constraint interfaces
#include "sfo_cpp/sfo_concepts/cost_function.hpp"
#include "sfo_cpp/sfo_concepts/constraint.hpp"

// Elements are templated out, include a basic "element" class for testing
#include "sfo_cpp/tests/test_utils/demo_element.hpp"

class ModularCost : public testing::Test
{
protected:
    void SetUp() override
    {
        // Initialize modular function weights.
        // weights are just i**2 for each index i.
        double value = 0;
        for (auto el : (*ground_set))
        {
            value++;
            weights.insert({el, value * value});
        }

        // Extract the optimal value and set (largest `budget` elements.)
        std::vector<std::pair<Element *, double>> top_elements(budget);
        // Quickly compute the optimal set (largest )
        std::partial_sort_copy(weights.begin(), weights.end(), top_elements.begin(), top_elements.end(), [](std::pair<Element *, double> const &l, std::pair<Element *, double> const &r)
                               { return l.second > r.second; });

        optimal_value = 0;
        optimal_set.clear();
        for (auto &[element, val] : top_elements)
        {
            optimal_value = optimal_value + val;
            optimal_set.insert(element);
        }
    }

    // Available test variables

    int set_size = 10;
    int budget = 3;
    // Ground set.
    std::unordered_set<Element *> *ground_set = generate_ground_set(set_size);

    // Cardinality constraint.
    constraint::Constraint<Element> *cardinality_constraint = new constraint::Cardinality<Element>(budget);

    // Modular cost function.
    std::unordered_map<Element *, double> weights;
    costfunction::CostFunction<Element> *cost_function = new costfunction::Modular<Element>(weights); // modular

    // Optimal cost and value for this case.
    std::unordered_set<Element *> optimal_set{};
    double optimal_value = 0;
};

class SqrtModularCost : public testing::Test
{
protected:
    void SetUp() override
    {
        // Initialize modular function weights.
        // weights are just i**2 for each index i.
        double value = 0;
        for (auto el : (*ground_set))
        {
            value++;
            weights.insert({el, value * value});
        }

        // Extract the optimal value and set (largest `budget` elements.)
        std::vector<std::pair<Element *, double>> top_elements(budget);
        // Quickly compute the optimal set (largest `budget` elements)
        std::partial_sort_copy(weights.begin(), weights.end(), top_elements.begin(), top_elements.end(), [](std::pair<Element *, double> const &l, std::pair<Element *, double> const &r)
                               { return l.second > r.second; });

        // Compute the optimal value (square root of modular cost)
        optimal_value = 0;
        optimal_set.clear();
        for (auto &[element, val] : top_elements)
        {
            optimal_value = optimal_value + val;
            optimal_set.insert(element);
        }
        optimal_value = sqrt(optimal_value);
    }

    // Available test variables

    int set_size = 10;
    int budget = 3;
    // Ground set.
    std::unordered_set<Element *> *ground_set = generate_ground_set(set_size);

    // Cardinality constraint.
    constraint::Constraint<Element> *cardinality_constraint = new constraint::Cardinality<Element>(budget);

    // Modular cost function.
    std::unordered_map<Element *, double> weights;
    costfunction::Modular<Element> modular = costfunction::Modular(weights);                           // modular part of cost
    costfunction::CostFunction<Element> *cost_function = new costfunction::SquareRootModular(modular); // square root of modular

    // Optimal cost and value for this case.
    std::unordered_set<Element *> optimal_set{};
    double optimal_value = 0;
};