#include <gtest/gtest.h>

#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>

// include the algorithms we want
#include "sfo_cpp/optimizers/monotone/vanilla_greedy.hpp"
#include "sfo_cpp/optimizers/monotone/lazy_greedy.hpp"
#include "sfo_cpp/optimizers/monotone/stochastic_greedy.hpp"
#include "sfo_cpp/optimizers/monotone/lazier_than_lazy_greedy.hpp"

// include the cost function and constraint interfaces
#include "sfo_cpp/sfo_concepts/cost_function.hpp"
#include "sfo_cpp/sfo_concepts/constraint.hpp"

// Elements are templated out, include a basic "element" class for testing
#include "sfo_cpp/tests/demo_element.hpp"

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
    costfunction::CostFunction<Element> *modular_cost = new costfunction::Modular<Element>(weights); // modular

    // Optimal cost and value for this case.
    std::unordered_set<Element *> optimal_set{};
    double optimal_value = 0;
};

TEST_F(ModularCost, VanillaGreedyTest)
{
    // Create an algorithm object.
    VanillaGreedy<Element> greedy;

    greedy.set_ground_set(ground_set);
    greedy.add_constraint(cardinality_constraint);
    greedy.set_cost_function(modular_cost);

    greedy.set_cost_benefit(false);

    greedy.run_greedy();

    // Constraint should be saturated.
    EXPECT_EQ(greedy.constraint_saturated, true);
    EXPECT_EQ(greedy.curr_set.size(), budget);

    // We should have the optimal cost, since the cost function is modular.
    ASSERT_NEAR(greedy.curr_val, optimal_value, std::numeric_limits<float>::epsilon()) << "Optimizer result: " << greedy.curr_val << " Optimal: " << optimal_value;
    EXPECT_EQ(greedy.curr_set, optimal_set) << "Optimizer set: " << greedy.curr_set << " Optimal: " << optimal_set;
}

TEST_F(ModularCost, VanillaGreedyCostBenefitTest)
{
    // Create an algorithm object.
    VanillaGreedy<Element> greedy;

    greedy.set_ground_set(ground_set);
    greedy.add_constraint(cardinality_constraint);
    greedy.set_cost_function(modular_cost);

    greedy.set_cost_benefit(true);

    greedy.run_greedy();

    // Constraint should be saturated.
    EXPECT_EQ(greedy.constraint_saturated, true);
    EXPECT_EQ(greedy.curr_set.size(), budget);

    // We should have the optimal cost, since the cost function is modular.
    ASSERT_NEAR(greedy.curr_val, optimal_value, std::numeric_limits<float>::epsilon()) << "Optimizer result: " << greedy.curr_val << " Optimal: " << optimal_value;
    EXPECT_EQ(greedy.curr_set, optimal_set) << "Optimizer set: " << greedy.curr_set << " Optimal: " << optimal_set;
}

TEST_F(ModularCost, LazyGreedyTest)
{
    // Create an algorithm object.
    LazyGreedy<Element> greedy;

    greedy.set_ground_set(ground_set);
    greedy.add_constraint(cardinality_constraint);
    greedy.set_cost_function(modular_cost);

    greedy.set_cost_benefit(false);

    greedy.run_greedy();

    // Constraint should be saturated.
    EXPECT_EQ(greedy.constraint_saturated, true);
    EXPECT_EQ(greedy.curr_set.size(), budget);

    // We should have the optimal cost, since the cost function is modular.
    ASSERT_NEAR(greedy.curr_val, optimal_value, std::numeric_limits<float>::epsilon()) << "Optimizer result: " << greedy.curr_val << " Optimal: " << optimal_value;
    EXPECT_EQ(greedy.curr_set, optimal_set) << "Optimizer set: " << greedy.curr_set << " Optimal: " << optimal_set;
}

TEST_F(ModularCost, LazyGreedyCostBenefitTest)
{
    // now, let's create an algorithm object to operate on that ground set.
    LazyGreedy<Element> greedy;

    greedy.set_ground_set(ground_set);
    greedy.add_constraint(cardinality_constraint);
    greedy.set_cost_function(modular_cost);

    greedy.set_cost_benefit(true);

    greedy.run_greedy();

    // Constraint should be saturated.
    EXPECT_EQ(greedy.constraint_saturated, true);
    EXPECT_EQ(greedy.curr_set.size(), budget);

    // We should have the optimal cost, since the cost function is modular.
    ASSERT_NEAR(greedy.curr_val, optimal_value, std::numeric_limits<float>::epsilon()) << "Optimizer result: " << greedy.curr_val << " Optimal: " << optimal_value;
    EXPECT_EQ(greedy.curr_set, optimal_set) << "Optimizer set: " << greedy.curr_set << " Optimal: " << optimal_set;
}

TEST_F(ModularCost, StochasticGreedyTest)
{
    // now, let's create an algorithm object to operate on that ground set.
    StochasticGreedy<Element> greedy;

    greedy.set_ground_set(ground_set);
    greedy.add_constraint(cardinality_constraint);
    greedy.set_cost_function(modular_cost);

    greedy.run_greedy();

    // Constraint should be saturated.
    EXPECT_EQ(greedy.constraint_saturated, true);
    EXPECT_EQ(greedy.curr_set.size(), budget);

    // We should have the optimal cost, since the cost function is modular.
    ASSERT_NEAR(greedy.curr_val, optimal_value, std::numeric_limits<float>::epsilon()) << "Optimizer result: " << greedy.curr_val << " Optimal: " << optimal_value;
    EXPECT_EQ(greedy.curr_set, optimal_set) << "Optimizer set: " << greedy.curr_set << " Optimal: " << optimal_set;
}

TEST_F(ModularCost, LazierThanLazyGreedyTest)
{
    // now, let's create an algorithm object to operate on that ground set.
    LazierThanLazyGreedy<Element> greedy;

    greedy.set_ground_set(ground_set);
    greedy.add_constraint(cardinality_constraint);
    greedy.set_cost_function(modular_cost);

    greedy.run_greedy();

    // Constraint should be saturated.
    EXPECT_EQ(greedy.constraint_saturated, true);
    EXPECT_EQ(greedy.curr_set.size(), budget);

    // We should have the optimal cost, since the cost function is modular.
    ASSERT_NEAR(greedy.curr_val, optimal_value, std::numeric_limits<float>::epsilon()) << "Optimizer result: " << greedy.curr_val << " Optimal: " << optimal_value;
    EXPECT_EQ(greedy.curr_set, optimal_set) << "Optimizer set: " << greedy.curr_set << " Optimal: " << optimal_set;
}