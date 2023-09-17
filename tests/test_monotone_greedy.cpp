#include <iostream>
#include <vector>

// include the algorithms we want
#include "sfo_cpp/monotone_algs/vanilla_greedy.hpp"
#include "sfo_cpp/monotone_algs/lazy_greedy.hpp"
#include "sfo_cpp/monotone_algs/stochastic_greedy.hpp"
#include "sfo_cpp/monotone_algs/lazier_than_lazy_greedy.hpp"

// include the cost function and constraint interfaces
#include "sfo_cpp/SFO_core/cost_function.hpp"
#include "sfo_cpp/SFO_core/constraint.hpp"

// Elements are templated out, include basic "element" class for testing
#include "sfo_cpp/SFO_core/element.hpp"


int main(){
    int set_size = 10;
    int budget = 3;
    // first, let's create a ground set
    std::unordered_set<Element*> *ground_set = generate_ground_set(set_size);

    // now, let's create some algorithm objects to operate on that ground set
    GreedyAlgorithm<Element> greedy(ground_set);
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
    greedy.add_constraint(card);
    lazygreedy.add_constraint(crd);
    stochasticgreedy.add_constraint(crd);
    lazier_than_lazy_greedy.add_constraint(crd);

    // run the algorithms one after another
    // modular cost function first (greedy algorithm is provably optimal)
    std::cout<< "Successfully built greedy algorithms." <<std::endl;
    std::cout<< std::endl << "****************RUNNING MODULAR COSTS*************" << std::endl;
    std::cout<<"==============VANILLA GREEDY==============" << std::endl;
    greedy.run_greedy(*modular);
    std::cout<<"==============VANILLA CB GREEDY==============" << std::endl;
    greedy.clear_set();
    greedy.run_greedy(*modular, true);
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
    greedy.run_greedy(*sqrtmodular);
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