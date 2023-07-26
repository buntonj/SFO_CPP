#include <unordered_set>
#include <set>
#include <iostream>
#include "vanilla_greedy.hpp"
#include "lazy_greedy.hpp"

double F(std::unordered_set<int> set){
    return set.size();
}

double H(std::set<Element> set){
    return set.size();
}

int main(){
    int set_size = 10;
    int budget = 3;
    GreedyAlgorithm greedy(set_size, budget);
    LazyGreedy lazygreedy(set_size, budget);
    std::cout<< "Successfully built greedy algorithms." <<std::endl;
    std::cout<< "Running vanilla greedy..." << std::endl;
    // greedy.run_greedy(&F);
    std::cout<< "Running lazy greedy..." << std::endl;
    lazygreedy.run_greedy(&H);
}