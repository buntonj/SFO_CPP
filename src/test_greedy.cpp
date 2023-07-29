#include <iostream>
#include <vector>
#include "vanilla_greedy.hpp"
#include "lazy_greedy.hpp"


int main(){
    int set_size = 10;
    int budget = 3;
    std::vector<double> weights;
    GreedyAlgorithm greedy(set_size, budget);
    LazyGreedy lazygreedy(set_size, budget);

    for(int i=0; i<set_size; i++){
        weights.push_back(float(i)*float(i));
    }

    CostFunction* cardinality = new Modular(weights);
    CostFunction* sqrtmodular = new SquareRootModular;
    std::cout<< "Successfully built greedy algorithms." <<std::endl;
    std::cout<< "Running vanilla greedy..." << std::endl;
    greedy.run_greedy(*sqrtmodular);
    std::cout<< "Running lazy greedy..." << std::endl;
    lazygreedy.run_greedy(*sqrtmodular);
}