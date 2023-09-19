#pragma once
#include <iostream>
#include <unordered_set>
#include <cfloat>
#include "sfo_cpp/SFO_core/cost_function.hpp"
#include "sfo_cpp/SFO_core/element.hpp"
#include "sfo_cpp/SFO_core/constraint.hpp"

template<typename E> class ApxLocalSearch{
    private:
        double curr_val = 0;  // current value of elements in set
        bool constraint_saturated = false;
        int MAXITER = 15;
        double epsilon;

    public:
        std::unordered_set<E*> *ground_set;  // pointer to ground set of elements
        int n = 0;  // holds size of ground set, indexed from 0 to n-1
        constraint::Constraint<E> *constraint;
        std::unordered_set<E*> curr_set;  // will hold elements selected to be in our set

        ApxLocalSearch(int &N){
            this->set_ground_set(generate_ground_set(N));
        };

        ApxLocalSearch(int &N, int &B){  // If you give a budget, initialize a budget constraint
            this->set_ground_set(generate_ground_set(N));
            this->add_constraint(new constraint::Cardinality<E>(B));
        };

        ApxLocalSearch(std::unordered_set<E*> *V){
            this->set_ground_set(V);
        };

        ApxLocalSearch(std::unordered_set<E*> *V, int &B){
            this->set_ground_set(V);
            this->add_constraint(new constraint::Cardinality<E>(B));
        };
        
        std::unordered_set<E*>* generate_ground_set(int &n){
            std::unordered_set<E*> *V = new std::unordered_set<E*>;
            int id = 0;
            double val = 0;
            for (int i=0; i<n; i++){
                id++;
                V->insert(new E(id, val));
            }
            return V;
        }

        void set_ground_set(std::unordered_set<E*> *V){
            this->ground_set = V;
            this->n = V->size();
        }

        void run_greedy(costfunction::CostFunction<E> &C, std::unordered_set<E*> *V, double epsilon){
            if(V->size() < 1){
                std::cout << "Ground set is empty!" << std::endl;
                return;
            } else{
                this->set_ground_set(V);
                this->run_greedy(C);
            }
        };

        void run_greedy(costfunction::CostFunction<E> &C, double epsilon){
            if(this->n < 1){
                std::cout<< "No ground set given!" << std::endl;
                return;
            } else {
                this->run_greedy(C, this->ground_set, epsilon);
            }
        };

        void print_status(){
            std::cout << "Current set:";
            std::cout<< curr_set;
            std::cout<<"Current val: " << curr_val << std::endl;
            std::cout<<"Constraint saturated? " << constraint_saturated << std::endl;
        };

        void add_constraint(constraint::Constraint<E> *C){
            constraint = C;
        }

        void clear_set(){
            this->curr_set.clear();
            this->curr_val = 0;
            this->constraint_saturated = false;
        }

    private:
        void greedy_step(costfunction::CostFunction<E> &F){
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
                candidate_marginal_val = F(test_set) - curr_val;

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

        void cost_benefit_greedy_step(costfunction::CostFunction<E> &F, constraint::Knapsack<E> *K, double &curr_budget){
            std::unordered_set <E*> test_set(curr_set);
            E* best_el;
            double best_marginal_val = -DBL_MAX;
            double best_marginal_cost = 1;
            double candidate_marginal_cost = 1;
            double candidate_marginal_val = -DBL_MAX;

            for(auto el=ground_set->begin(); el != ground_set->end(); ++el){
                // note that el is a POINTER to a POINTER to an element in the ground set
                test_set = curr_set;

                // if element is already in our set, skip it
                if (curr_set.find(*el) != curr_set.end()){
                    continue;
                }

                test_set.insert(*el);

                // if new element violates the constraint, skip it
                if (! constraint->test_membership(test_set)){
                    continue;
                }

                // update marginal value
                candidate_marginal_cost = K->value(test_set) - curr_budget;
                candidate_marginal_val = F(test_set) - curr_val;

                // keep running track of highest marginal value element
                if (candidate_marginal_val/candidate_marginal_cost > best_marginal_val/best_marginal_cost){
                    best_el = *el;
                    best_marginal_val = candidate_marginal_val;
                    best_marginal_cost = candidate_marginal_cost;
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