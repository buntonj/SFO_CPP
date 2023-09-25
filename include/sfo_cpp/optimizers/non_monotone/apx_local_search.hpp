#pragma once
#include <iostream>
#include <unordered_set>
#include <cfloat>
#include "../../sfo_concepts/element.hpp"
#include "../../sfo_concepts/cost_function.hpp"
#include "../../sfo_concepts/constraint.hpp"

template<typename E> class ApxLocalSearch{
    private:
        double curr_val = 0;  // current value of elements in set
        bool constraint_saturated = false;
        int MAXITER = 15;
        double epsilon = 0;

    public:
        std::unordered_set<E*> *ground_set;  // pointer to ground set of elements
        int n = 0;  // holds size of ground set, indexed from 0 to n-1
        std::unordered_set<constraint::Constraint<E>*> constraint_set;
        costfunction::CostFunction<E> *cost_function;
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

        void clear_set(){
            this->curr_set.clear();
            this->curr_val = 0;
            this->constraint_saturated = false;
        }

        void run_greedy(){
            if(this->n < 1){
                std::cout<<"No ground set given!" <<std::endl;
                return;
            }

            // check if the constraint is compatible--cannot be knapsack
            if(constraint::Knapsack<E>* k = dynamic_cast<constraint::Knapsack<E>*>(constraint); k != nullptr){
                std::cout<<"Constraint is a knapsack, apx local search is only valid with matroids."
                return;
            }

            if(this->epsilon <= 0){
                std::cout<<"Epsilon not set, using default value 0.25."<<std::endl;
                this->epsilon = 0.25;
            }

            for(int i = 0; i < this->num_constraints; i++){
                // ththis is a comment!
                // this is another comment!
                //code code codenn
            }
            int counter=0;
            // details of greedy algorithm here
        }

        void print_status(){
            std::cout << "Current set:";
            std::cout<< curr_set;
            std::cout<<"Current val: " << curr_val << std::endl;
            std::cout<<"Constraint saturated? " << constraint_saturated << std::endl;
        };

    private:
        void greedy_step(){
            std::unordered_set <E*> test_set(curr_set);
            E* best_el;
            double best_marginal_val = -DBL_MAX;
            double candidate_marginal_val = 0;

            for(auto el=ground_set->begin(); el != ground_set->end(); ++el){
                // note that el is a POINTER to a POINTER to an element in the ground set
                test_set = curr_set;

                if (curr_set.find(*el) != curr_set.end()){
                    continue;
                }

                test_set.insert(*el);

                if (! constraint->test_membership(test_set)){
                    continue;
                }

                // update marginal value
                candidate_marginal_val = cost_function->operator()(test_set) - curr_val;

                // keep running track of highest marginal value element
                if (candidate_marginal_val > best_marginal_val){
                    best_el = *el;
                    best_marginal_val = candidate_marginal_val;
                }
            }

            // check if we could even add an element to set
            if (best_marginal_val < 0){
                constraint_saturated = true;  // no more elements could be feasibly added
            } else{
                // update the current set, value, and budget value with the found item
                curr_set.insert(best_el);
                curr_val = curr_val + best_marginal_val;
                constraint_saturated = constraint->is_saturated(curr_set);  // check if constraint is now saturated
            }
        };
};