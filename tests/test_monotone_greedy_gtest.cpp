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

// Elements are templated out, include basic "element" class for testing
#include "sfo_cpp/sfo_concepts/element.hpp"

class ModularCost : public testing::Test{
    protected:
        void SetUp() override {
            // Initialize modular function weights.
            // weights are just i**2 for each index i.
            double value = 0;
            for(auto el:(*ground_set)){
                value++;
                weights.insert({el, value*value});
            }

            // Extract the optimal value and set (largest `budget` elements.)
            std::vector<std::pair<Element*, double>> top_elements(budget);
            // Quickly compute the optimal set (largest )
            std::partial_sort_copy(weights.begin(), weights.end(), top_elements.begin(), top_elements.end(),[](std::pair<Element*, double> const& l,
                            std::pair<Element*, double> const& r)
                        {
                            return l.second > r.second;
                        });

            optimal_value = 0;
            optimal_set.clear();
            for(auto &[element, val]:top_elements){
                optimal_value = optimal_value + val;
                optimal_set.insert(element);
            }
        }

        // Available test variables
        
        int set_size = 10;
        int budget = 3;
        // Ground set.
        std::unordered_set<Element*> *ground_set = generate_ground_set(set_size);
        
        // Cardinality constraint.
        constraint::Constraint<Element>* cardinality_constraint = new constraint::Cardinality<Element>(budget);

        // Modular cost function.
        std::unordered_map<Element*, double> weights;
        costfunction::CostFunction<Element>* modular_cost = new costfunction::Modular<Element>(weights); // modular

        // Optimal cost and value for this case.
        std::unordered_set<Element*> optimal_set{};
        double optimal_value = 0;

};

TEST_F(ModularCost, VanillaGreedy){
    // now, let's create an algorithm object to operate on that ground set.
    GreedyAlgorithm<Element> greedy;

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

TEST_F(ModularCost, VanillaGreedyCostBenefit){
    // now, let's create an algorithm object to operate on that ground set.
    GreedyAlgorithm<Element> greedy;

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

TEST_F(ModularCost, LazyGreedyTest){
    // now, let's create an algorithm object to operate on that ground set.
    LazyGreedyAlgorithm<Element> greedy;

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

TEST_F(ModularCost, LazyGreedyCostBenefitTest){
    // now, let's create an algorithm object to operate on that ground set.
    LazyGreedyAlgorithm<Element> greedy;

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

TEST_F(ModularCost, StochasticGreedy){
    // now, let's create an algorithm object to operate on that ground set.
    StochasticGreedyAlgorithm<Element> greedy;

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

// int main(){
//     int set_size = 10;
//     int budget = 3;
//     // first, let's create a ground set
//     std::unordered_set<Element*> *ground_set = generate_ground_set(set_size);

//     // now, let's create some algorithm objects to operate on that ground set
//     GreedyAlgorithm<Element> greedy;
//     LazyGreedyAlgorithm<Element> lazygreedy;
//     StochasticGreedyAlgorithm<Element> stochasticgreedy;
//     LazierThanLazyGreedy<Element> ltlgreedy;
//     double epsilon = 0.05;  /// parameter needed for stochastic/lazier than lazy algorithms

//     // define weights for a modular function
//     std::unordered_map<Element*, double> weights;
//     for(auto el:(*ground_set)){
//         weights.insert({el, float(el->id)*float(el->id)});
//     }

//     // Next, build some cost functions
//     costfunction::CostFunction<Element>* cardinality = new costfunction::Modular<Element>;  //cardinality
//     costfunction::CostFunction<Element>* modular = new costfunction::Modular<Element>(weights); // modular
//     costfunction::CostFunction<Element>* sqrtmodular = new costfunction::SquareRootModular<Element>; // square root modular

//     // then, we build some constraints to add to the problems
//     constraint::Constraint<Element>* card = new constraint::Knapsack<Element>(budget); // cardinality at knapsack level
//     constraint::Constraint<Element>* crd = new constraint::Cardinality<Element>(budget);  // cardinality as subclass

//     // New simpler interface
//     // slightly more verbose but WAY more clear what is happening
//     // "CVX-like"
//     greedy.set_ground_set(ground_set);
//     greedy.add_constraint(crd);
//     greedy.set_cost_benefit(false);

//     lazygreedy.set_ground_set(ground_set);
//     lazygreedy.add_constraint(crd);
//     lazygreedy.set_cost_benefit(false);

//     stochasticgreedy.set_ground_set(ground_set);
//     stochasticgreedy.add_constraint(crd);
//     stochasticgreedy.set_epsilon(epsilon);

//     ltlgreedy.set_ground_set(ground_set);
//     ltlgreedy.add_constraint(crd);
//     ltlgreedy.set_epsilon(epsilon);

//     // run the algorithms one after another
//     // modular cost function first (greedy algorithm is provably optimal)
//     std::cout<< "Successfully built greedy algorithms." <<std::endl;
//     std::cout<< std::endl << "****************RUNNING MODULAR COSTS*************" << std::endl;
//     std::cout<<"==============VANILLA GREEDY==============" << std::endl;
//     greedy.set_cost_function(modular);
//     greedy.run_greedy();
//     std::cout<<"==============VANILLA CB GREEDY==============" << std::endl;
//     greedy.clear_set();
//     greedy.set_cost_benefit(true);
//     greedy.run_greedy();
//     std::cout<<"==============LAZY GREEDY==============" << std::endl;
//     lazygreedy.set_cost_function(modular);
//     lazygreedy.run_greedy();
//     std::cout<<"==============LAZY CB GREEDY==============" << std::endl;
//     lazygreedy.clear_set();
//     lazygreedy.set_cost_benefit(true);
//     lazygreedy.run_greedy();
//     std::cout<<"==============STOCHASTIC GREEDY==============" << std::endl;
//     stochasticgreedy.clear_set();
//     stochasticgreedy.set_cost_function(modular);
//     stochasticgreedy.run_greedy();
//     std::cout<<"==============STOCHASTIC LAZY GREEDY==============" << std::endl;
//     ltlgreedy.clear_set();
//     ltlgreedy.set_cost_function(modular);
//     ltlgreedy.run_greedy();

//     // square root modular (strictly submodular cost, greedy algorithm near-optimal)
//     std::cout<< std::endl << "******************SQRT MODULAR COST*****************" << std::endl;
//     std::cout<<"==============VANILLA GREEDY==============" << std::endl;
//     greedy.clear_set();
//     greedy.set_cost_function(sqrtmodular);
//     greedy.set_cost_benefit(false);
//     greedy.run_greedy();
//     std::cout<<"==============LAZY GREEDY==============" << std::endl;
//     lazygreedy.clear_set();
//     lazygreedy.set_cost_function(sqrtmodular);
//     lazygreedy.set_cost_benefit(false);
//     lazygreedy.run_greedy();
//     std::cout<<"==============STOCHASTIC GREEDY==============" << std::endl;
//     stochasticgreedy.clear_set();
//     stochasticgreedy.set_cost_function(sqrtmodular);
//     stochasticgreedy.run_greedy();
//     std::cout<<"==============STOCHASTIC LAZY GREEDY==============" << std::endl;
//     ltlgreedy.clear_set();
//     ltlgreedy.set_cost_function(sqrtmodular);
//     ltlgreedy.run_greedy();

//     // cleanup stack memory
//     delete cardinality;
//     delete modular;
//     delete sqrtmodular;
//     delete card;
//     delete crd;
// }