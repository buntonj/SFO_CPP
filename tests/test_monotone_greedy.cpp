#include <iostream>
#include <vector>

// include the algorithms we want
#include "sfo_cpp/optimizers/monotone/vanilla_greedyV2.hpp"
#include "sfo_cpp/optimizers/monotone/lazy_greedy.hpp"
#include "sfo_cpp/optimizers/monotone/stochastic_greedy.hpp"
#include "sfo_cpp/optimizers/monotone/lazier_than_lazy_greedy.hpp"

// include the cost function and constraint interfaces
#include "sfo_cpp/sfo_concepts/cost_function.hpp"
#include "sfo_cpp/sfo_concepts/constraint.hpp"

// Elements are templated out, include basic "element" class for testing
#include "sfo_cpp/sfo_concepts/element.hpp"


int main(){
    int set_size = 10;
    int budget = 3;
    // first, let's create a ground set
    std::unordered_set<Element*> *ground_set = generate_ground_set(set_size);

    // now, let's create some algorithm objects to operate on that ground set
    GreedyAlgorithm<Element> greedy;
    greedy.set_ground_set(ground_set);
    LazyGreedy lazygreedy(ground_set);
    StochasticGreedyAlgorithm stochasticgreedy(ground_set);
    LazierThanLazyGreedy lazier_than_lazy_greedy(ground_set);
    double epsilon = 0.25;  /// parameter needed for stochastic/lazier than lazy algorithms

    // define weights for a modular function
    std::unordered_map<Element*, double> weights;
    for(auto el:(*ground_set)){
        weights.insert({el, float(el->id)*float(el->id)});
    }

    // Next, build some cost functions
    costfunction::CostFunction<Element>* cardinality = new costfunction::Modular<Element>;  //cardinality
    costfunction::CostFunction<Element>* modular = new costfunction::Modular<Element>(weights); // modular
    costfunction::CostFunction<Element>* sqrtmodular = new costfunction::SquareRootModular<Element>; // square root modular

    // then, we build some constraints to add to the problems
    constraint::Constraint<Element>* card = new constraint::Knapsack<Element>(budget); // cardinality at knapsack level
    constraint::Constraint<Element>* crd = new constraint::Cardinality<Element>(budget);  // cardinality as subclass
    
    // add cardinality constraints to the algorithm objects
    greedy.set_constraint(card);
    lazygreedy.set_constraint(crd);
    stochasticgreedy.set_constraint(crd);
    lazier_than_lazy_greedy.set_constraint(crd);

    // run the algorithms one after another
    // modular cost function first (greedy algorithm is provably optimal)
    std::cout<< "Successfully built greedy algorithms." <<std::endl;
    std::cout<< std::endl << "****************RUNNING MODULAR COSTS*************" << std::endl;
    std::cout<<"==============VANILLA GREEDY==============" << std::endl;
    greedy.set_cost_function(modular);
    greedy.run_greedy();
    std::cout<<"==============VANILLA CB GREEDY==============" << std::endl;
    greedy.clear_set();
    greedy.set_cost_benefit(true);
    greedy.run_greedy();
    std::cout<<"==============LAZY GREEDY==============" << std::endl;
    lazygreedy.run_greedy(*modular);
    std::cout<<"==============LAZY CB GREEDY==============" << std::endl;
    lazygreedy.clear_set();
    lazygreedy.run_greedy(*modular, true);
    std::cout<<"==============STOCHASTIC GREEDY==============" << std::endl;
    stochasticgreedy.clear_set();
    stochasticgreedy.run_greedy(*modular, epsilon);
    std::cout<<"==============STOCHASTIC LAZY GREEDY==============" << std::endl;
    lazier_than_lazy_greedy.clear_set();
    lazier_than_lazy_greedy.run_greedy(*modular, epsilon);

    // square root modular (strictly submodular cost, greedy algorithm near-optimal)
    std::cout<< std::endl << "******************SQRT MODULAR COST*****************" << std::endl;
    std::cout<<"==============VANILLA GREEDY==============" << std::endl;
    greedy.clear_set();
    greedy.set_cost_function(sqrtmodular);
    greedy.set_cost_benefit(false);
    greedy.run_greedy();
    std::cout<<"==============LAZY GREEDY==============" << std::endl;
    lazygreedy.clear_set();
    lazygreedy.run_greedy(*sqrtmodular);
    std::cout<<"==============STOCHASTIC GREEDY==============" << std::endl;
    stochasticgreedy.clear_set();
    stochasticgreedy.run_greedy(*sqrtmodular, epsilon);
    std::cout<<"==============STOCHASTIC LAZY GREEDY==============" << std::endl;
    lazier_than_lazy_greedy.clear_set();
    lazier_than_lazy_greedy.run_greedy(*sqrtmodular, epsilon);

    // cleanup stack memory
    delete cardinality;
    delete modular;
    delete sqrtmodular;
    delete card;
    delete crd;
}