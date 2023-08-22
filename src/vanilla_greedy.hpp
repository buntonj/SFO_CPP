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
        constraint::Constraint *constraint;
        bool constraint_saturated = false;
        int MAXITER = 15;
        std::unordered_set<Element*> *ground_set;  // pointer to ground set of elements
        std::unordered_map<Element*, double> marginal_gains;  // will hold marginal benefit of each element

    public:
        int n = 0;  // holds size of ground set, indexed from 0 to n-1
        std::unordered_set <Element> curr_set;  // will hold elements selected to be in our set

        GreedyAlgorithm(int &N){
            n = N;
        };

        GreedyAlgorithm(int &N, int &B){  // If you give a budget, initialize a budget constraint
            n = N;
            add_constraint(new constraint::Knapsack(B));
        };

        GreedyAlgorithm(std::unordered_set<Element*> *V){
            this->n = V->size();
            this->ground_set = V;
        };

        GreedyAlgorithm(std::unordered_set<Element*> *V, int &B){
            this->n = V->size();
            this->ground_set = V;
            this->add_constraint(new constraint::Knapsack(B));
        };

        void set_ground_set(std::unordered_set<Element*> *V){
            this->ground_set = V;
            this->n = V->size();
        }

        void run_greedy(costfunction::CostFunction &C, std::unordered_set<Element*> *V){
        //  Can only call greedy in this way if it already knows about a non-empty ground set
            if(V->size() < 1){
                std::cout << "Ground set is empty!" << endl;
                return;
            } else{
                this->set_ground_set(V);
                this->run_greedy(C);
            }
        }

        void run_greedy(costfunction::CostFunction &C){
            if(this->n < 1){
                std::cout << "No ground set given!" << std::endl;  //should be replaced with throw
            }
            int counter=0;
            while (!constraint_saturated && counter < MAXITER){
                counter++;
                greedy_step(C);
                std::cout<< "Performed greedy algorithm iteration: " << counter << std::endl;
                print_status();
            }
        };

        void run_greedy(costfunction::CostFunction &C, bool cost_benefit){
            if(!cost_benefit){
                // if not asking for cost-benefit alg, run vanilla greedy
                run_greedy(C);
            } else if (constraint::Knapsack* k = dynamic_cast<constraint::Knapsack*>(constraint); k != nullptr){
                // if asking for cost-benefit, check that constraint is a knapsack one
                // if it is, k becomes a pointer to derived Constraint::Knapsack type
                int counter=0;
                double budget = 0;
                while (!constraint_saturated && counter < MAXITER){
                    counter++;
                    cost_benefit_greedy_step(C, k, budget);
                    std::cout<< "Performed greedy algorithm iteration: " << counter << std::endl;
                    print_status();
                }
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
        void greedy_step(costfunction::CostFunction &F){
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