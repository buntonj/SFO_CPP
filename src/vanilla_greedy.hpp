#pragma once
#include <iostream>
#include <unordered_set>
#include <cfloat>
#include "cost_function.hpp"
#include "element.hpp"

class GreedyAlgorithm{
    private:
        int curr_budget = 0;  // number of elements currently in set
        double curr_val = 0;  // current value of elements in set

    public:
        int n;  // holds size of ground set, indexed from 0 to n-1
        int budget;
        std::unordered_set <Element> curr_set;  // will hold elements selected to be in our set

        GreedyAlgorithm(int &N, int &B){
            n = N;
            budget = B;
        };

        void run_greedy(CostFunction &C){
            int counter=0;
            while (curr_budget < budget){
                counter++;
                greedy_step(C);
                std::cout<< "Performed greedy algorithm iteration: " << counter << std::endl;
                print_status();
            }
        };

        void print_status(){
            std::cout << "Current set:";
            std::cout<< curr_set;
            std::cout<<"Current val: " << curr_val << std::endl;
        };

    private:
        void greedy_step(CostFunction &F){
            std::unordered_set <Element> test_set(curr_set);
            // double marginal_val = -DBL_MAX;
            // double new_marginal_val;
            Element candidate;
            Element best(0, -DBL_MAX);

            for(int i=0; i < n; i++){
                candidate.id = i;
                test_set.insert(candidate);  // add element to test set

                candidate.set_value(F(test_set) - curr_val);  // compute its value
                std::cout<< "Test set:" <<std::endl;
                std::cout << test_set << std::endl;
                std::cout << "Marginal val:" << candidate.value << std::endl;

                // if it is the best right now, remember it
                best = std::max(best, candidate);
                //if (candidate.value > best.value){
                //    best = candidate;
                //}

                // remove it from test set
                test_set = curr_set;  // Not the most efficient way to do this rn
            }

            // update the current set, value, and budget value with the found item
            curr_set.insert(best);
            curr_val = curr_val + best.value;
            curr_budget++;
        };
};