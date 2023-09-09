#pragma once
#include <queue>
#include <unordered_set>
#include <iostream>
#include <vector>
#include <cfloat>
#include "SFO_core/element.hpp"
#include "SFO_core/cost_function.hpp"
#include "SFO_core/constraint.hpp"


class LazyGreedy{
    private:
        double curr_val = 0;  // current value of elements in set
        bool constraint_saturated = false;
        int MAXITER = 15;

    public:
        std::unordered_set<Element*> *ground_set;
        int n;  // holds size of ground set, indexed from 0 to n-1
        std::unordered_set<Element*> curr_set;  // will hold elements selected to be in our set
        LazyGreedyQueue marginals; // will hold marginals
        constraint::Constraint *constraint;

        LazyGreedy(int &N){
            this->set_ground_set(this->generate_ground_set(N));
        };

        LazyGreedy(int &N, int &B){  // If you give a budget, initialize a budget constraint
            this->set_ground_set(this->generate_ground_set(N));
            this->add_constraint(new constraint::Cardinality(B));
        };

        LazyGreedy(std::unordered_set<Element*> *V){
            this->set_ground_set(V);
        };

        LazyGreedy(std::unordered_set<Element*> *V, int &B){
            this->set_ground_set(V);
            this->add_constraint(new constraint::Cardinality(B));
        };

        std::unordered_set<Element*>* generate_ground_set(int &n){
            std::unordered_set<Element*> *V = new std::unordered_set<Element*>;
            int id = 0;
            double val = 0;
            for (int i=0; i<n; i++){
                id++;
                V->insert(new Element(id, val));
            }
            return V;
        }

        void set_ground_set(std::unordered_set<Element*> *V){
            this->ground_set = V;
            this->n = V->size();
        }

        void clear_set(){
            this->curr_set.clear();
            this->curr_val = 0;
            this->constraint_saturated = false;
            this->clear_marginals();
        }

        void run_greedy(costfunction::CostFunction &F, std::unordered_set<Element*> *V, bool cost_benefit){
            if(V->size() < 1){
                std::cout<<"Ground set is empty!" <<std::endl;
                return;
            } else{
                this->set_ground_set(V);
                this->run_greedy(F, cost_benefit);
            }
        }

        void run_greedy(costfunction::CostFunction &F, bool cost_benefit){
            if (this->n < 1){
                std::cout<< "No ground set given!" << std::endl;
                return;
            }
            if(!cost_benefit){
                run_greedy(F);
            } else if(constraint::Knapsack* K = dynamic_cast<constraint::Knapsack*>(constraint); K != nullptr){
                int counter = 1;
                double budget = 0;
                clear_marginals();
                cost_benefit_first_iteration(F, K, budget);  // initializes marginals in first greedy iteration
                std::cout<< "Performed CB LAZY GREEDY algorithm iteration: " << counter << std::endl;
                print_status();
                while (!constraint_saturated && counter < MAXITER){
                    counter++;
                    cost_benefit_lazy_greedy_step(F, K, budget);
                    std::cout<< "Performed CB LAZY GREEDY algorithm iteration: " << counter << std::endl;
                    print_status();
                }
            }
        };

        void run_greedy(costfunction::CostFunction &cost_function){
            clear_marginals();
            first_iteration(cost_function);  // initializes marginals in first greedy iteration
            int counter=1;
            std::cout<< "Performed LAZY GREEDY algorithm iteration: " << counter << std::endl;
            print_status();
            while (!constraint_saturated && counter < MAXITER){
                counter++;
                lazy_greedy_step(cost_function);
                std::cout<< "Performed LAZY GREEDY algorithm iteration: " << counter << std::endl;
                print_status();
            }
        };

        void print_status(){
            std::cout << "Current set:" << curr_set << std::endl;
            std::cout<<"Current val: " << curr_val << std::endl;
            std::cout<<"Constraint saturated? " << constraint_saturated << std::endl;
        };

        void add_constraint(constraint::Constraint *C){
            constraint = C;
        }

    private:

        // Special function for first iteration, populates priority queue
        void first_iteration(costfunction::CostFunction &F){
            std::unordered_set<Element*> test_set(curr_set);
            std::pair<Element*, double> candidate(nullptr, -DBL_MAX);

            for(auto el=ground_set->begin(); el != ground_set->end(); ++el){
                test_set = curr_set;

                // check if element in set yet
                if(curr_set.find(*el) != curr_set.end()){
                    continue;
                }

                // put element in testing set
                test_set.insert(*el);

                // if test set violates constraint, skip it
                if(!constraint->test_membership(test_set)){
                    continue;
                }

                // build out candidate pair
                candidate.first = *el;
                candidate.second = F(test_set) - curr_val;

                marginals.push(candidate);

            }

            if (! marginals.empty()){
                // check that there is an element
                candidate = marginals.top();
                if (candidate.second > 0){
                    // check that its added value is positive
                    curr_set.insert(candidate.first);
                    curr_val = curr_val + candidate.second;
                    marginals.pop();
                    constraint_saturated = constraint->is_saturated(curr_set);
                } else {
                    // if no element provided positive gain we are done
                    constraint_saturated = true;
                }
            } else {
                // if no element then we are done
                constraint_saturated = true;
            }
        }

        // Special function for first iteration, populates priority queue
        void cost_benefit_first_iteration(costfunction::CostFunction &F, constraint::Knapsack *K, double &curr_budget){
            std::unordered_set<Element*> test_set(curr_set);
            std::unordered_map<Element*, double> pure_vals;
            std::unordered_map<Element*, double> pure_knaps;
            std::pair<Element*, double> candidate;

            for(auto el=ground_set->begin(); el != ground_set->end(); ++el){
                test_set = curr_set;

                // check if element in set yet
                if(curr_set.find(*el) != curr_set.end()){
                    continue;
                }

                test_set.insert(*el);
                
                if(!constraint->test_membership(test_set)){
                    continue;
                }
                
                candidate.first = *el;
                pure_vals.insert({candidate.first, F(test_set) - curr_val});
                pure_knaps.insert({candidate.first, K->value(test_set) - curr_budget});
                candidate.second = pure_vals[candidate.first]/pure_knaps[candidate.first];

                marginals.push(candidate);  // insert into priority queue

            }

            if (! marginals.empty()){
                // check that there is a feasible element to add
                if (auto best = marginals.top(); best.second > 0){
                    // check that its added value is positive
                    curr_set.insert(best.first);  // add it to set
                    constraint_saturated = constraint->is_saturated(curr_set);  // update constraint saturation
                    curr_val = curr_val + pure_vals[best.first]; // update current value
                    curr_budget = curr_budget + pure_knaps[best.first];
                    marginals.pop(); // pop element from queue
                } else {
                    // if no element added value, we are done
                    constraint_saturated = true;
                }
            } else {
                // if no feasible element then we are done
                constraint_saturated = true;
            }
        }

        void lazy_greedy_step(costfunction::CostFunction &F){
            std::unordered_set <Element*> test_set(curr_set);
            std::pair<Element*, double> candidate;

            // iterate through all candidate element IDs
            while(! marginals.empty()){
                test_set = curr_set;

                // pull first element from priority queue
                candidate.first = marginals.top().first;
                test_set.insert(candidate.first);  // add it to testing set

                if (!constraint->test_membership(test_set)){
                    marginals.pop();
                    continue;  // leave element out from now on
                }

                candidate.second = F(test_set)-curr_val;

                // put updated candidate back into priority queue
                marginals.pop();
                marginals.push(candidate);

                // if it is still at the top, we have found best element
                if (marginals.top().second <= candidate.second){
                    break;
                }
            }

            if(! marginals.empty()){
                if (auto best = marginals.top(); best.second > 0){
                    // update the current set, value, and budget value with the found item
                    curr_set.insert(best.first);
                    curr_val = curr_val + best.second;
                    marginals.pop();
                    constraint_saturated = constraint->is_saturated(curr_set); // allows for early stop detection
                } else {
                    constraint_saturated = true;
                }
            } else{
                constraint_saturated = true;
            }
        };

        void cost_benefit_lazy_greedy_step(costfunction::CostFunction &F, constraint::Knapsack* K, double curr_budget){
            std::unordered_set <Element*> test_set(curr_set);
            std::unordered_map<Element*, double> pure_vals;
            std::unordered_map<Element*, double> pure_knaps;
            std::pair<Element*, double> candidate;

                        // iterate through all candidate element IDs
            while(! marginals.empty()){
                test_set = curr_set;

                // pull first element from priority queue
                candidate.first = marginals.top().first;
                test_set.insert(candidate.first);  // add it to testing set

                marginals.pop();

                if (!constraint->test_membership(test_set)){
                    continue;  // leave element out from now on
                }

                pure_vals.insert({candidate.first, F(test_set)-curr_val});
                pure_knaps.insert({candidate.first, K->value(test_set) - curr_budget});
                candidate.second = pure_vals[candidate.first]/pure_knaps[candidate.first];


                // put updated candidate back into priority queue
                marginals.push(candidate);

                // if it is still at the top, we have found best element
                if (marginals.top().second <= candidate.second){
                    break;
                }
            }

            if(! marginals.empty()){
                if (auto best = marginals.top(); best.second > 0){
                    // update the current set, value, and budget value with the found item
                    curr_set.insert(best.first);
                    curr_val = curr_val + pure_vals[best.first];
                    curr_budget = curr_budget + pure_knaps[best.first];
                    marginals.pop();
                    constraint_saturated = constraint->is_saturated(curr_set); // allows for early stop detection
                } else {
                    constraint_saturated = true;
                }
            } else{
                constraint_saturated = true;
            }
        };

        void clear_marginals(){
            LazyGreedyQueue empty;
            std::swap(marginals, empty);
        }
};