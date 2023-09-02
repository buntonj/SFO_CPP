#pragma once
#include <iostream>
#include <unordered_set>
#include <cfloat>
#include <cmath>
#include <random>
#include "cost_function.hpp"
#include "element.hpp"
#include "constraint.hpp"

class StochasticGreedyAlgorithm{
    private:
        double curr_val = 0;  // current value of elements in set
        int b;
        constraint::Constraint *constraint;
        bool constraint_saturated = false;
        int MAXITER = 15;

    public:
        int n;  // holds size of ground set, indexed from 0 to n-1
        std::unordered_set<Element> curr_set;  // will hold elements selected to be in our set

        StochasticGreedyAlgorithm(int &N){
            n = N;
        };

        StochasticGreedyAlgorithm(int &N, int &B){  // If you give a budget, initialize a budget constraint
            n = N;
            b = B;
            add_constraint(new constraint::Cardinality(B));
        };

        void run_greedy(costfunction::CostFunction &C, double epsilon){
            int sample_size = random_set_size(epsilon, n, double(b));
            int counter=0;
            while (!constraint_saturated && counter < MAXITER){
                counter++;
                stochastic_greedy_step(C, sample_size);
                std::cout<< "Performed greedy algorithm iteration: " << counter << std::endl;
                print_status();
            }
        };

        void run_greedy(costfunction::CostFunction &C, double epsilon, bool cost_benefit){
            // stochastic greedy is only valid for cardinality constraints, so check
            if (constraint::Cardinality* k = dynamic_cast<constraint::Cardinality*>(constraint); k != nullptr){
                // if asking for cost-benefit, check that constraint is a knapsack one
                // if it is, k becomes a pointer to derived Constraint::Knapsack type
                int counter=0;
                double budget = 0;
                while (!constraint_saturated && counter < MAXITER){
                    counter++;
                    stochastic_greedy_step(C, budget);
                    std::cout<< "Performed greedy algorithm iteration: " << counter << std::endl;
                    print_status();
                }
            } else {
                std::cout<<"Constraint is not a cardinality constraint, stochastic greedy is not valid."<<std::endl;
            }
        };

        void print_status(){
            std::cout << "Current set:";
            std::cout<< curr_set;
            std::cout<<"Current val: " << curr_val << std::endl;
            std::cout<<"Constraint saturated? " << constraint_saturated << std::endl;
        };

        void add_constraint(constraint::Constraint *C){
            constraint = C;
        }

    private:
        int random_set_size(double epsilon, int ground_set_size, double budget){
            return int(std::min((double(ground_set_size)/budget)*log(1.0/epsilon), double(ground_set_size)));
        }
        void stochastic_greedy_step(costfunction::CostFunction &F, int sample_size){
            std::unordered_set <Element> test_set(curr_set);
            Element candidate;
            Element best(0, -DBL_MAX);

            // randomly sample a set of size sample_size
            // compute the marginal gains for the elements in that set
            // choose the max gain element from the set

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

        void cost_benefit_greedy_step(costfunction::CostFunction &F, constraint::Knapsack *K, double &curr_budget){
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

                candidate.set_value((F(test_set) - curr_val)/(K->value(candidate)));  // compute its value
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
                curr_budget = K->value(curr_set); // update the budget
                curr_val = curr_val + best.value;
                constraint_saturated = constraint->is_saturated(curr_set);  // check if constraint is now saturated
            }
        };
};