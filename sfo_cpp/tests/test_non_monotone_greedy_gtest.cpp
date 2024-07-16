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
    std::cerr << "Optimizer result: " << greedy.curr_val << " Optimal: " << optimal_value;
    ASSERT_NEAR(greedy.curr_val, optimal_value, std::numeric_limits<float>::epsilon()) << "Optimizer result: " << greedy.curr_val << " Optimal: " << optimal_value;
    EXPECT_EQ(greedy.curr_set, optimal_set) << "Optimizer set: " << greedy.curr_set << " Optimal: " << optimal_set;
}