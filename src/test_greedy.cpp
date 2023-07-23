#include <set>
#include <iostream>
#include "vanilla_greedy.hpp"

double F(std::set<int> set){
    return set.size();
}

int main(){
    int set_size = 10;
    int budget = 3;
    GreedyAlgorithm greedy(set_size, budget);
    std::cout<< "Successfully built greedy algorithm." <<std::endl;
    greedy.run_greedy(&F);
}