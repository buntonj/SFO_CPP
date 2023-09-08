#include <iostream>
#include <vector>
#include "sfo_cpp/vanilla_greedy.hpp"
#include "sfo_cpp/lazy_greedy.hpp"
#include "sfo_cpp/stochastic_greedy.hpp"
#include "sfo_cpp/SFO_core/constraint.hpp"


int main(){
    int set_size = 10;
    int budget = 3;
    GreedyAlgorithm greedy(set_size);
    std::unordered_set<Element*> *ground_set = greedy.ground_set;
    LazyGreedy lazygreedy(ground_set);
    StochasticGreedyAlgorithm stochasticgreedy(ground_set);
    double epsilon = 0.01;

    std::unordered_map<Element*, double> weights;
    int i = 0;
    for(auto el:(*ground_set)){
        i++;
        weights.insert({el, float(i)*float(i)});
    }

    costfunction::CostFunction* cardinality = new costfunction::Modular;
    costfunction::CostFunction* modular = new costfunction::Modular(weights);
    costfunction::CostFunction* sqrtmodular = new costfunction::SquareRootModular;
    constraint::Constraint* card = new constraint::Knapsack(budget);
    constraint::Constraint* crd = new constraint::Cardinality(budget);
    
    greedy.add_constraint(card); // add the cardinality constraint
    lazygreedy.add_constraint(crd);
    stochasticgreedy.add_constraint(crd);
    // lazygreedy.set_ground_set(greedy.ground_set);
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

    // cleanup
    delete cardinality;
    delete modular;
    delete sqrtmodular;
    delete card;
    delete crd;
}