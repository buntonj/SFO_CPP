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
#include "sfo_cpp/tests/test_utils/demo_element.hpp"
#include "sfo_cpp/tests/test_utils/test_fixtures.hpp"

TEST_F(ConstrainedModularCost, VanillaGreedyTest)
{
    // Create an algorithm object.
    VanillaGreedy<Element> greedy;

    greedy.set_ground_set(ground_set);
    greedy.add_constraint(cardinality_constraint);
    greedy.set_cost_function(cost_function);

    greedy.set_cost_benefit(false);

    greedy.run_greedy();

    // Constraint should be saturated.
    EXPECT_TRUE(greedy.constraint_saturated);
    EXPECT_EQ(greedy.curr_set.size(), budget);

    // We should have the optimal cost, since the cost function is modular.
    ASSERT_FLOAT_EQ(greedy.curr_val, optimal_value) << "Optimizer result: " << greedy.curr_val << " Optimal: " << optimal_value;
    EXPECT_EQ(greedy.curr_set, optimal_set) << "Optimizer set: " << greedy.curr_set << " Optimal: " << optimal_set;
}

TEST_F(ConstrainedModularCost, VanillaGreedyCostBenefitTest)
{
    // Create an algorithm object.
    VanillaGreedy<Element> greedy;

    greedy.set_ground_set(ground_set);
    greedy.add_constraint(cardinality_constraint);
    greedy.set_cost_function(cost_function);

    greedy.set_cost_benefit(true);

    greedy.run_greedy();

    // Constraint should be saturated.
    EXPECT_EQ(greedy.constraint_saturated, true);
    EXPECT_EQ(greedy.curr_set.size(), budget);

    // We should have the optimal cost, since the cost function is modular.
    ASSERT_NEAR(greedy.curr_val, optimal_value, std::numeric_limits<float>::epsilon()) << "Optimizer result: " << greedy.curr_val << " Optimal: " << optimal_value;
    EXPECT_EQ(greedy.curr_set, optimal_set) << "Optimizer set: " << greedy.curr_set << " Optimal: " << optimal_set;
}

TEST_F(ConstrainedModularCost, LazyGreedyTest)
{
    // Create an algorithm object.
    LazyGreedy<Element> greedy;

    greedy.set_ground_set(ground_set);
    greedy.add_constraint(cardinality_constraint);
    greedy.set_cost_function(cost_function);

    greedy.set_cost_benefit(false);

    greedy.run_greedy();

    // Constraint should be saturated.
    EXPECT_EQ(greedy.constraint_saturated, true);
    EXPECT_EQ(greedy.curr_set.size(), budget);

    // We should have the optimal cost, since the cost function is modular.
    ASSERT_NEAR(greedy.curr_val, optimal_value, std::numeric_limits<float>::epsilon()) << "Optimizer result: " << greedy.curr_val << " Optimal: " << optimal_value;
    EXPECT_EQ(greedy.curr_set, optimal_set) << "Optimizer set: " << greedy.curr_set << " Optimal: " << optimal_set;
}

TEST_F(ConstrainedModularCost, LazyGreedyCostBenefitTest)
{
    // now, let's create an algorithm object to operate on that ground set.
    LazyGreedy<Element> greedy;

    greedy.set_ground_set(ground_set);
    greedy.add_constraint(cardinality_constraint);
    greedy.set_cost_function(cost_function);

    greedy.set_cost_benefit(true);

    greedy.run_greedy();

    // Constraint should be saturated.
    EXPECT_EQ(greedy.constraint_saturated, true);
    EXPECT_EQ(greedy.curr_set.size(), budget);

    // We should have the optimal cost, since the cost function is modular.
    ASSERT_NEAR(greedy.curr_val, optimal_value, std::numeric_limits<float>::epsilon()) << "Optimizer result: " << greedy.curr_val << " Optimal: " << optimal_value;
    EXPECT_EQ(greedy.curr_set, optimal_set) << "Optimizer set: " << greedy.curr_set << " Optimal: " << optimal_set;
}

TEST_F(ConstrainedModularCost, StochasticGreedyTest)
{
    // now, let's create an algorithm object to operate on that ground set.
    StochasticGreedy<Element> greedy;

    greedy.set_ground_set(ground_set);
    greedy.add_constraint(cardinality_constraint);
    greedy.set_cost_function(cost_function);

    greedy.run_greedy();

    // Constraint should be saturated.
    EXPECT_EQ(greedy.constraint_saturated, true);
    EXPECT_EQ(greedy.curr_set.size(), budget);

    // We should have the optimal cost, since the cost function is modular.
    ASSERT_NEAR(greedy.curr_val, optimal_value, std::numeric_limits<float>::epsilon()) << "Optimizer result: " << greedy.curr_val << " Optimal: " << optimal_value;
    EXPECT_EQ(greedy.curr_set, optimal_set) << "Optimizer set: " << greedy.curr_set << " Optimal: " << optimal_set;
}

TEST_F(ConstrainedModularCost, LazierThanLazyGreedyTest)
{
    // now, let's create an algorithm object to operate on that ground set.
    LazierThanLazyGreedy<Element> greedy;

    greedy.set_ground_set(ground_set);
    greedy.add_constraint(cardinality_constraint);
    greedy.set_cost_function(cost_function);

    greedy.run_greedy();

    // Constraint should be saturated.
    EXPECT_EQ(greedy.constraint_saturated, true);
    EXPECT_EQ(greedy.curr_set.size(), budget);

    // We should have the optimal cost, since the cost function is modular.
    ASSERT_NEAR(greedy.curr_val, optimal_value, std::numeric_limits<float>::epsilon()) << "Optimizer result: " << greedy.curr_val << " Optimal: " << optimal_value;
    EXPECT_EQ(greedy.curr_set, optimal_set) << "Optimizer set: " << greedy.curr_set << " Optimal: " << optimal_set;
}

// Tests for monotone square root modular cost.

TEST_F(SqrtModularCost, VanillaGreedyTest)
{
    // Create an algorithm object.
    LazyGreedy<Element> greedy;

    greedy.set_ground_set(ground_set);
    greedy.add_constraint(cardinality_constraint);
    greedy.set_cost_function(cost_function);

    greedy.set_cost_benefit(false);

    greedy.run_greedy();

    // Constraint should be saturated.
    EXPECT_EQ(greedy.constraint_saturated, true);
    EXPECT_EQ(greedy.curr_set.size(), budget);

    // We should have the optimal cost, since the cost function is modular.
    ASSERT_NEAR(greedy.curr_val, optimal_value, std::numeric_limits<float>::epsilon()) << "Optimizer result: " << greedy.curr_val << " Optimal: " << optimal_value;
    EXPECT_EQ(greedy.curr_set, optimal_set) << "Optimizer set: " << greedy.curr_set << " Optimal: " << optimal_set;
}

TEST_F(SqrtModularCost, VanillaGreedyCostBenefitTest)
{
    // Create an algorithm object.
    LazyGreedy<Element> greedy;

    greedy.set_ground_set(ground_set);
    greedy.add_constraint(cardinality_constraint);
    greedy.set_cost_function(cost_function);

    greedy.set_cost_benefit(true);

    greedy.run_greedy();

    // Constraint should be saturated.
    EXPECT_EQ(greedy.constraint_saturated, true);
    EXPECT_EQ(greedy.curr_set.size(), budget);

    // We should have the optimal cost, since the cost function is modular.
    ASSERT_NEAR(greedy.curr_val, optimal_value, std::numeric_limits<float>::epsilon()) << "Optimizer result: " << greedy.curr_val << " Optimal: " << optimal_value;
    EXPECT_EQ(greedy.curr_set, optimal_set) << "Optimizer set: " << greedy.curr_set << " Optimal: " << optimal_set;
}