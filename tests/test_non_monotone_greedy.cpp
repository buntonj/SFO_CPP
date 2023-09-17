#include <iostream>
#include <vector>

// include the algorithms we want
#include "sfo_cpp/non_monotone_algs/bidirectional_greedy.hpp"

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
    BidirectionalGreedy bidirectional_greedy(ground_set);

    // define weights for a modular function
    std::unordered_map<Element*, double> weights;
    for(auto el:(*ground_set)){
        weights.insert({el, float(el->id)*float(el->id)});
    }

    // Next, build some cost functions
    costfunction::CostFunction<Element>* cardinality = new costfunction::Modular<Element>;  //cardinality
    costfunction::CostFunction<Element>* modular = new costfunction::Modular<Element>(weights); // modular
    costfunction::CostFunction<Element>* sqrtmodular = new costfunction::SquareRootModular<Element>; // square root modular
    double bias = double(set_size/2);
    double high = std::sqrt(set_size-bias);
    costfunction::CostFunction<Element>* centeredsqrtmodular = new costfunction::CenteredSqrtModular<Element>(bias, high); // square root modular

    // then, we build some constraints to add to the problems
    constraint::Constraint<Element>* card = new constraint::Knapsack<Element>(budget); // cardinality at knapsack level
    constraint::Constraint<Element>* crd = new constraint::Cardinality<Element>(budget);  // cardinality as subclass

    // run the algorithms one after another
    // modular cost function first (greedy algorithm is provably optimal)
    std::cout<< "Successfully built greedy algorithms." <<std::endl;
    std::cout<< std::endl << "****************RUNNING MODULAR COSTS*************" << std::endl;
    std::cout<<"==============BIDIRECTIONAL GREEDY==============" << std::endl;
    bidirectional_greedy.clear_set();
    bidirectional_greedy.run_greedy(*modular);
    std::cout<<"==============RANDOMIZED BIDIRECTIONAL GREEDY==============" << std::endl;
    bidirectional_greedy.clear_set();
    bidirectional_greedy.run_randomized_greedy(*modular);

    // square root modular (strictly submodular cost, greedy algorithm near-optimal)
    std::cout<< std::endl << "******************SQRT MODULAR COST*****************" << std::endl;
    std::cout<<"==============BIDIRECTIONAL GREEDY==============" << std::endl;
    bidirectional_greedy.clear_set();
    bidirectional_greedy.run_greedy(*sqrtmodular);
    std::cout<<"==============RANDOMIZED BIDIRECTIONAL GREEDY==============" << std::endl;
    bidirectional_greedy.clear_set();
    bidirectional_greedy.run_randomized_greedy(*sqrtmodular);

    // square root modular (strictly submodular cost, greedy algorithm near-optimal)
    std::cout<< std::endl << "******************(CENTERED) SQRT MODULAR COST*****************" << std::endl;
    std::cout<<"==============BIDIRECTIONAL GREEDY==============" << std::endl;
    bidirectional_greedy.clear_set();
    bidirectional_greedy.run_greedy(*centeredsqrtmodular);
    std::cout<<"==============RANDOMIZED BIDIRECTIONAL GREEDY==============" << std::endl;
    bidirectional_greedy.clear_set();
    bidirectional_greedy.run_randomized_greedy(*centeredsqrtmodular);

    // cleanup stack memory
    delete cardinality;
    delete modular;
    delete sqrtmodular;
    delete card;
    delete crd;
}