#include <gtest/gtest.h>

#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>

// include the algorithms we want
#include "sfo_cpp/optimizers/non_monotone/bidirectional_greedy.hpp"

// include the cost function and constraint interfaces
#include "sfo_cpp/sfo_concepts/cost_function.hpp"
#include "sfo_cpp/sfo_concepts/constraint.hpp"

// Elements are templated out, include a basic "element" class for testing
#include "sfo_cpp/tests/test_utils/demo_element.hpp"

// Convenience fixtures for testing various cost functions
#include "sfo_cpp/tests/test_utils/test_fixtures.hpp"

TEST_F(ConstrainedModularCost, BidirectionalGreedyTest)
{
    // Create an algorithm object.
    BidirectionalGreedy<Element> greedy;

    greedy.set_ground_set(ground_set);
    greedy.set_cost_function(cost_function);

    greedy.run_greedy();

    // We should have the optimal cost, since the cost function is modular.
    // Since the problem is unconstrained and monotone modular, the optimal should just be all elements.
    EXPECT_FLOAT_EQ(greedy.curr_val, cost_function->operator()(*ground_set)) << "Optimizer result: " << greedy.curr_val << " Optimal: " << optimal_value;
    EXPECT_EQ(greedy.curr_set, *ground_set) << "Optimizer set: " << greedy.curr_set << " Optimal: " << optimal_set;
}

TEST_F(SqrtModularCost, BidirectionalGreedyTest)
{
    // Create an algorithm object.
    BidirectionalGreedy<Element> greedy;

    greedy.set_ground_set(ground_set);
    greedy.set_cost_function(cost_function);

    greedy.run_greedy();

    // We should have the optimal cost, since the cost function is modular.
    // Since the problem is unconstrained and monotone modular, the optimal should just be all elements.
    EXPECT_FLOAT_EQ(greedy.curr_val, cost_function->operator()(*ground_set)) << "Optimizer result: " << greedy.curr_val << " Optimal: " << optimal_value;
    EXPECT_EQ(greedy.curr_set, *ground_set) << "Optimizer set: " << greedy.curr_set << " Optimal: " << optimal_set;
}

TEST_F(ConstrainedModularCost, RandomizedBidirectionalGreedyTest)
{
    // Create an algorithm object.
    BidirectionalGreedy<Element> greedy;

    greedy.set_ground_set(ground_set);
    greedy.set_cost_function(cost_function);
    greedy.set_randomized(true);

    greedy.run_greedy();

    // We should have the optimal cost, since the cost function is modular.
    // Since the problem is unconstrained and monotone modular, the optimal should just be all elements.
    EXPECT_FLOAT_EQ(greedy.curr_val, cost_function->operator()(*ground_set)) << "Optimizer result: " << greedy.curr_val << " Optimal: " << optimal_value;
    EXPECT_EQ(greedy.curr_set, *ground_set) << "Optimizer set: " << greedy.curr_set << " Optimal: " << optimal_set;
}

TEST_F(SqrtModularCost, RandomizedBidirectionalGreedyTest)
{
    // Create an algorithm object.
    BidirectionalGreedy<Element> greedy;

    greedy.set_ground_set(ground_set);
    greedy.set_cost_function(cost_function);

    greedy.run_greedy();

    // We should have the optimal cost, since the cost function is modular.
    // Since the problem is unconstrained and monotone modular, the optimal should just be all elements.
    EXPECT_FLOAT_EQ(greedy.curr_val, cost_function->operator()(*ground_set)) << "Optimizer result: " << greedy.curr_val << " Optimal: " << optimal_value;
    EXPECT_EQ(greedy.curr_set, *ground_set) << "Optimizer set: " << greedy.curr_set << " Optimal: " << optimal_set;
}