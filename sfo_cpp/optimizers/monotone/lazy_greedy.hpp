#pragma once
#include <queue>
#include <unordered_set>
#include <iostream>
#include <vector>
#include <cfloat>
#include "../../sfo_concepts/element.hpp"
#include "../../sfo_concepts/cost_function.hpp"
#include "../../sfo_concepts/constraint.hpp"


template<typename E> class LazyGreedy{
    private:
        int MAXITER = 15;
        LazyGreedyQueue<E> marginals; // will hold marginals

    public:
        double curr_val = 0;  // current value of elements in set
        bool constraint_saturated = false;
        std::unordered_set<E*> *ground_set;  // pointer to ground set of elements
        int n = 0;  // holds size of ground set, indexed from 0 to n-1
        std::unordered_set<constraint::Constraint<E>*> constraint_set;
        costfunction::CostFunction<E> *cost_function;
        bool cost_benefit = false;
        std::unordered_set<E*> curr_set;  // will hold elements selected to be in our set

        void set_ground_set(std::unordered_set<E*> *V){
            this->ground_set = V;
            this->n = V->size();
        }
        
        void add_constraint(constraint::Constraint<E> *C){
            this->constraint_set.insert(C);
        }

        void remove_constraint(constraint::Constraint<E> *C){
            this->constraint_set.erase(C);
        }

        bool check_constraints(std::unordered_set<E*> set){
            for(auto iter = constraint_set.begin(); iter != constraint_set.end(); ++iter){
                if(! ((*iter)->test_membership(set))){
                    // if any constraint is not satisfied, then intersection of them is not
                    return false;
                }
            }
            return true;  //if all constraints were satisfied, then return true
        }

        bool check_saturated(std::unordered_set<E*> set){
            for(auto iter = constraint_set.begin(); iter != constraint_set.end(); ++iter){
                if((*iter)->is_saturated(set)){
                    // if any constraint is saturated, then so is intersection
                    return true;
                }
            }
            return false;  // if no constraints were saturated, then return true
        }

        void set_cost_function(costfunction::CostFunction<E> *F){
            this->cost_function = F;
        }
        
        void set_cost_benefit(bool cb){
            this->cost_benefit = cb;
        }

        void clear_set(){
            this->curr_set.clear();
            this->curr_val = 0;
            this->constraint_saturated = false;
            this->clear_marginals();
        }

        void run_greedy(){
            if (this->n < 1){
                std::cout<< "No ground set given!" << std::endl;
                return;
            }

            clear_marginals();
            if(!cost_benefit){
                first_iteration();  // initializes marginals in first greedy iteration
                int counter = 1;
                std::cout<< "Performed LAZY GREEDY algorithm iteration: " << counter << std::endl;
                print_status();
                while (!constraint_saturated && counter < MAXITER){
                    counter++;
                    lazy_greedy_step();
                    std::cout<< "Performed LAZY GREEDY algorithm iteration: " << counter << std::endl;
                    print_status();
                }
            } else if(constraint::Knapsack<E>*K = find_single_knapsack(); K != nullptr){
                int counter = 1;
                double budget = 0;
                cost_benefit_first_iteration(K, budget);  // initializes marginals in first greedy iteration
                std::cout<< "Performed CB LAZY GREEDY algorithm iteration: " << counter << std::endl;
                print_status();
                while (!constraint_saturated && counter < MAXITER){
                    counter++;
                    cost_benefit_lazy_greedy_step(K, budget);
                    std::cout<< "Performed CB LAZY GREEDY algorithm iteration: " << counter << std::endl;
                    print_status();
                }
            } else {
                std::cout<< "Requested CB LAZY GREEDY with invalid constraint type." << std::endl;
            }
        };

        void print_status(){
            std::cout << "Current set:" << curr_set << std::endl;
            std::cout<<"Current val: " << curr_val << std::endl;
            std::cout<<"Constraint saturated? " << constraint_saturated << std::endl;
        };

    private:

        // Special function for first iteration, populates priority queue
        void first_iteration(){
            std::unordered_set<E*> test_set(curr_set);
            std::pair<E*, double> candidate(nullptr, -DBL_MAX);

            for(auto el=ground_set->begin(); el != ground_set->end(); ++el){
                test_set = curr_set;

                // check if element in set yet
                if(curr_set.find(*el) != curr_set.end()){
                    continue;
                }

                // put element in testing set
                test_set.insert(*el);

                // if test set violates constraint, skip it
                if(!this->check_constraints(test_set)){
                    continue;
                }

                // build out candidate pair
                candidate.first = *el;
                candidate.second = cost_function->operator()(test_set) - curr_val;

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
                    constraint_saturated = this->check_saturated(curr_set);
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
        void cost_benefit_first_iteration(constraint::Knapsack<E> *K, double &curr_budget){
            std::unordered_set<E*> test_set(curr_set);
            std::unordered_map<E*, double> pure_vals;
            std::unordered_map<E*, double> pure_knaps;
            std::pair<E*, double> candidate;

            for(auto el=ground_set->begin(); el != ground_set->end(); ++el){
                test_set = curr_set;

                // check if element in set yet
                if(curr_set.find(*el) != curr_set.end()){
                    continue;
                }

                test_set.insert(*el);
                
                if(!this->check_constraints(test_set)){
                    continue;
                }
                
                candidate.first = *el;
                pure_vals.insert({candidate.first, cost_function->operator()(test_set) - curr_val});
                pure_knaps.insert({candidate.first, K->value(test_set) - curr_budget});
                candidate.second = pure_vals[candidate.first]/pure_knaps[candidate.first];

                marginals.push(candidate);  // insert into priority queue

            }

            if (! marginals.empty()){
                // check that there is a feasible element to add
                if (auto best = marginals.top(); best.second > 0){
                    // check that its added value is positive
                    curr_set.insert(best.first);  // add it to set
                    constraint_saturated = this->check_saturated(curr_set);  // update constraint saturation
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

        void lazy_greedy_step(){
            std::unordered_set <E*> test_set(curr_set);
            std::pair<E*, double> candidate;

            // iterate through all candidate element IDs
            while(! marginals.empty()){
                test_set = curr_set;

                // pull first element from priority queue
                candidate.first = marginals.top().first;
                test_set.insert(candidate.first);  // add it to testing set

                if (!this->check_constraints(test_set)){
                    marginals.pop();
                    continue;  // leave element out from now on
                }

                candidate.second = cost_function->operator()(test_set)-curr_val;

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
                    constraint_saturated = this->check_saturated(curr_set); // allows for early stop detection
                } else {
                    constraint_saturated = true;
                }
            } else{
                constraint_saturated = true;
            }
        };

        void cost_benefit_lazy_greedy_step(constraint::Knapsack<E>* K, double curr_budget){
            std::unordered_set <E*> test_set(curr_set);
            std::unordered_map<E*, double> pure_vals;
            std::unordered_map<E*, double> pure_knaps;
            std::pair<E*, double> candidate;

                        // iterate through all candidate element IDs
            while(! marginals.empty()){
                test_set = curr_set;

                // pull first element from priority queue
                candidate.first = marginals.top().first;
                test_set.insert(candidate.first);  // add it to testing set

                marginals.pop();

                if (!this->check_constraints(test_set)){
                    continue;  // leave element out from now on
                }

                pure_vals.insert({candidate.first, cost_function->operator()(test_set)-curr_val});
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
                    constraint_saturated = this->check_saturated(curr_set); // allows for early stop detection
                } else {
                    constraint_saturated = true;
                }
            } else{
                constraint_saturated = true;
            }
        };

        void clear_marginals(){
            LazyGreedyQueue<E> empty;
            std::swap(marginals, empty);
        }

        constraint::Knapsack<E>* find_single_knapsack(){
            constraint::Knapsack<E>* knapsack_ptr;
            for (auto it=constraint_set.begin(); it != constraint_set.end(); ++it){
                // iterate over constraints in set, looking for one that can be cast to knapsack
                knapsack_ptr = dynamic_cast<constraint::Knapsack<E>*>(*it);
                if (knapsack_ptr != nullptr){
                    // if we find one, return it (just the first one found)
                    return knapsack_ptr;
                }
            }
            // if we didn't find one, then return nullptr
            return nullptr;
        }
};