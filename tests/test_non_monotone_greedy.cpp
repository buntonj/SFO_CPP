#include <iostream>
#include <vector>

// include the algorithms we want
#include "sfo_cpp/optimizers/non_monotone/bidirectional_greedy.hpp"

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
    BidirectionalGreedy<Element> bdgreedy;

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
    bdgreedy.set_ground_set(ground_set);
    bdgreedy.set_randomized(false);
    std::cout<< "Successfully built greedy algorithms." <<std::endl;

    std::cout<< std::endl << "****************RUNNING MODULAR COSTS*************" << std::endl;
    std::cout<<"==============BIDIRECTIONAL GREEDY==============" << std::endl;
    bdgreedy.clear_set();
    bdgreedy.run_greedy();
    std::cout<<"==============RANDOMIZED BIDIRECTIONAL GREEDY==============" << std::endl;
    bdgreedy.clear_set();
    bdgreedy.set_randomized(true);
    bdgreedy.run_greedy();


    // square root modular (strictly submodular cost, greedy algorithm near-optimal)
    std::cout<< std::endl << "******************SQRT MODULAR COST*****************" << std::endl;
    bdgreedy.set_cost_function(sqrtmodular);

    std::cout<<"==============BIDIRECTIONAL GREEDY==============" << std::endl;
    bdgreedy.clear_set();
    bdgreedy.set_randomized(false);
    bdgreedy.run_greedy();
    std::cout<<"==============RANDOMIZED BIDIRECTIONAL GREEDY==============" << std::endl;
    bdgreedy.clear_set();
    bdgreedy.set_randomized(true);
    bdgreedy.run_greedy();

    // square root modular (strictly submodular cost, greedy algorithm near-optimal)
    std::cout<< std::endl << "******************(CENTERED) SQRT MODULAR COST*****************" << std::endl;
    bdgreedy.set_cost_function(centeredsqrtmodular);

    std::cout<<"==============BIDIRECTIONAL GREEDY==============" << std::endl;
    bdgreedy.clear_set();
    bdgreedy.set_randomized(false);
    bdgreedy.run_greedy();
    std::cout<<"==============RANDOMIZED BIDIRECTIONAL GREEDY==============" << std::endl;
    bdgreedy.clear_set();
    bdgreedy.set_randomized(true);
    bdgreedy.run_greedy();

    // cleanup stack memory
    delete cardinality;
    delete modular;
    delete sqrtmodular;
    delete card;
    delete crd;
}