#include <iostream>
#include <vector>
#include "sfo_cpp/vanilla_greedy.hpp"
#include "sfo_cpp/lazy_greedy.hpp"
#include "sfo_cpp/stochastic_greedy.hpp"
#include "sfo_cpp/lazier_than_lazy_greedy.hpp"
#include "sfo_cpp/SFO_core/constraint.hpp"


int main(){
    int set_size = 10;
    int budget = 3;
    std::unordered_set<Element*> *ground_set = generate_ground_set(set_size);
    GreedyAlgorithm<Element> greedy(ground_set);
    // std::unordered_set<Element*> *ground_set = greedy.ground_set;
    LazyGreedy lazygreedy(ground_set);
    StochasticGreedyAlgorithm stochasticgreedy(ground_set);
    LazierThanLazyGreedy lazier_than_lazy_greedy(ground_set);
    double epsilon = 0.25;

    std::unordered_map<Element*, double> weights;
    for(auto el:(*ground_set)){
        weights.insert({el, float(el->id)*float(el->id)});
    }

    costfunction::CostFunction<Element>* cardinality = new costfunction::Modular<Element>;
    costfunction::CostFunction<Element>* modular = new costfunction::Modular<Element>(weights);
    costfunction::CostFunction<Element>* sqrtmodular = new costfunction::SquareRootModular<Element>;
    constraint::Constraint<Element>* card = new constraint::Knapsack<Element>(budget);
    constraint::Constraint<Element>* crd = new constraint::Cardinality<Element>(budget);
    
    greedy.add_constraint(card); // add the cardinality constraint
    lazygreedy.add_constraint(crd);
    stochasticgreedy.add_constraint(crd);
    lazier_than_lazy_greedy.add_constraint(crd);
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
    std::cout<<"==============STOCHASTIC LAZY GREEDY==============" << std::endl;
    lazier_than_lazy_greedy.clear_set();
    lazier_than_lazy_greedy.run_greedy(*modular, epsilon);

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

    // cleanup
    delete cardinality;
    delete modular;
    delete sqrtmodular;
    delete card;
    delete crd;
}