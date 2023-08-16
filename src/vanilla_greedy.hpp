#pragma once
#include <iostream>
#include <unordered_set>
#include <cfloat>
#include "cost_function.hpp"
#include "element.hpp"
#include "constraint.hpp"

class GreedyAlgorithm{
    private:
        double curr_val = 0;  // current value of elements in set
        Constraint *constraint;
        bool constraint_saturated = false;
        int MAXITER = 15;

    public:
        int n;  // holds size of ground set, indexed from 0 to n-1
        std::unordered_set <Element> curr_set;  // will hold elements selected to be in our set

        GreedyAlgorithm(int &N){
            n = N;
        };

        GreedyAlgorithm(int &N, int &B){  // If you give a budget, initialize a budget constraint
            n = N;
            add_constraint(new Knapsack(B));
        };

        void run_greedy(CostFunction &C){
            int counter=0;
            while (!constraint_saturated && counter < MAXITER){
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
            std::cout<<"Constraint saturated? " << constraint_saturated << std::endl;
        };

        void add_constraint(Constraint *C){
            constraint = C;
        }

    private:
        void greedy_step(CostFunction &F){
            std::unordered_set <Element> test_set(curr_set);
            Element candidate;
            Element best(0, -DBL_MAX);

            for(int i=0; i < n; i++){
                test_set = curr_set;
                // build new candidate element
                candidate.id = i;
                // if the candidate is in the set already, continue
                if (test_set.find(candidate) != test_set.end()){
                    continue;
                }

                // add the candidate element to the current set
                test_set.insert(candidate);

                // check if adding the element violates the constraint
                if(! constraint->test_membership(test_set)){
                    continue;
                }

                candidate.set_value(F(test_set) - curr_val);  // compute its value
                std::cout<< "Test set:" <<std::endl;
                std::cout << test_set << std::endl;
                std::cout << "Marginal val:" << candidate.value << std::endl;

                // if it is the best right now, remember it
                best = std::max(best, candidate);
            }

            // check if we could even add an element to set
            if (best.value < 0){
                constraint_saturated = true;  // no more elements could be feasibly added
            } else{
                // update the current set, value, and budget value with the found item
                curr_set.insert(best);
                curr_val = curr_val + best.value;
                constraint_saturated = constraint->is_saturated(curr_set);  // check if constraint is now saturated
            }
        };
};