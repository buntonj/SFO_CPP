#include <iostream>
#include <vector>
#include "vanilla_greedy.hpp"
#include "lazy_greedy.hpp"
#include "constraint.hpp"


int main(){
    int set_size = 10;
    int budget = 3;
    std::vector<double> weights;
    GreedyAlgorithm greedy(set_size);
    LazyGreedy lazygreedy(set_size);

    for(int i=0; i<set_size; i++){
        weights.push_back(float(i)*float(i));
    }

    CostFunction* cardinality = new Modular;
    CostFunction* modular = new Modular(weights);
    CostFunction* sqrtmodular = new SquareRootModular;
    Constraint* card = new Knapsack(budget);
    greedy.add_constraint(card); // add the cardinality constraint
    lazygreedy.add_constraint(card);
    std::cout<< "Successfully built greedy algorithms." <<std::endl;
    std::cout<< "Running vanilla greedy..." << std::endl;
    greedy.run_greedy(*modular);
    std::cout<< "Running vanilla CB..." << std::endl;
    greedy.run_greedy(*modular, true);
    std::cout<< "Running lazy greedy..." << std::endl;
    lazygreedy.run_greedy(*modular);
    std::cout<< "Running lazy CB..." << std::endl;
    lazygreedy.run_greedy(*modular, true);
    // greedy.run_greedy(*sqrtmodular);
    // std::cout<< "Running lazy greedy..." << std::endl;
    // lazygreedy.run_greedy(*sqrtmodular);

    // cleanup
    delete cardinality;
    delete modular;
    delete sqrtmodular;
    delete card;
}