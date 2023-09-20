#pragma once
#include <iostream>
#include <unordered_set>
#include <cfloat>
#include <random>
#include "../../sfo_concepts/element.hpp"
#include "../../sfo_concepts/cost_function.hpp"
#include "../../sfo_concepts/constraint.hpp"

template<typename E> class BidirectionalGreedy{
    private:
        double curr_val = 0;  // current value of elements in set
        int MAXITER = 15;
        bool randomized = false;

    public:
        std::unordered_set<E*> *ground_set;  // pointer to ground set of elements
        int n = 0;  // holds size of ground set, indexed from 0 to n-1
        std::unordered_set<E*> top_set;
        double top_val = 0;
        std::unordered_set<E*> bottom_set;  // will hold elements selected to be in our set
        double bottom_val = 0;
        std::unordered_set<E*> curr_set;

        BidirectionalGreedy(int &N){
            this->set_ground_set(generate_ground_set(N));
        };

        BidirectionalGreedy(std::unordered_set<E*> *V){
            this->set_ground_set(V);
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
        };

        void set_ground_set(std::unordered_set<E*> *V){
            this->ground_set = V;
            this->n = V->size();
        };

        void run_greedy(costfunction::CostFunction<E> &C, std::unordered_set<E*> *V){
            if(V->size() < 1){
                std::cout << "Ground set is empty!" << std::endl;
                return;
            } else{
                this->set_ground_set(V);
                this->randomized = false;
                this->run_greedy(C);
            }
        };

        void run_randomized_greedy(costfunction::CostFunction<E> &C, std::unordered_set<E*> *V){
            if(V->size() < 1){
                std::cout << "Ground set is empty!" << std::endl;
                return;
            } else{
                this->set_ground_set(V);
                this->randomized = true;
                this->run_greedy(C);
            }
        };

        void run_randomized_greedy(costfunction::CostFunction<E> &C){
            if(ground_set->size() < 1){
                std::cout << "Ground set is empty!" << std::endl;
                return;
            } else{
                this->randomized = true;
                this->run_greedy(C);
            }
        };

        void run_greedy(costfunction::CostFunction<E> &C){
            //  Can only call greedy in this way if it already knows about a non-empty ground set
            if(this->n < 1){
                std::cout << "No ground set given!" << std::endl;  //should be replaced with throw
                return;
            } else{
                this->top_set = *ground_set;
                this->bottom_set.clear();
                this->top_val = C(top_set);
                this->bottom_val = 0;
                this->MAXITER = this->n;
                int counter=0;
                for(auto it = ground_set->begin(); it != ground_set->end(); ++it){
                    counter++;
                    greedy_step(C, *it);
                    if(this->randomized){
                        std::cout<< "Performed RANDOMIZED BIDIRECTIONAL greedy algorithm iteration: " << counter << std::endl;
                    } else {
                        std::cout<< "Performed BIDIRECTIONAL greedy algorithm iteration: " << counter << std::endl;
                    }
                    this->curr_set = (top_val > bottom_val) ? top_set : bottom_set;
                    this->curr_val = std::max(top_val, bottom_val);

                    if (this->top_set == this->bottom_set){
                        break;
                    }

                    print_status();
                }
            }
        };

        void print_status(){
            std::cout << "Current set:";
            std::cout<< curr_set;
            std::cout<<"Current val: " << curr_val << std::endl;
        };

        void clear_set(){
            this->curr_set.clear();
            this->bottom_set.clear();
            this->top_val = 0;
            this->bottom_val = 0;
        };

    private:
        void greedy_step(costfunction::CostFunction<E> &F, E* el){
            std::unordered_set <E*> test_set(bottom_set);
            double bottom_gain, top_gain;

            test_set = bottom_set;

            test_set.insert(el);
            bottom_gain = F(test_set) - bottom_val;

            test_set = top_set;
            test_set.erase(el);
            top_gain = F(test_set) - top_val;
            
            if (this->randomized){
                // randomized version
                double denom = std::max(0.0, bottom_gain) + std::max(0.0, top_gain);
                if(denom == 0.0){
                    // in this case, we default to the bottom set
                    bottom_set.insert(el);
                    bottom_val = bottom_val + bottom_gain;
                } else {
                    // otherwise, we do a weighted randomized draw
                    if(double(std::rand())/RAND_MAX <= std::max(0.0, bottom_gain)/denom){
                        bottom_set.insert(el);
                        bottom_val = bottom_val + bottom_gain;
                    } else {
                        top_set.erase(el);
                        top_val = top_val + top_gain;
                    }
                }

            } else {
                if (bottom_gain >= top_gain){
                    // then add the element to the bottom set
                    bottom_set.insert(el);
                    // update current bottom set value
                    bottom_val = bottom_val + bottom_gain;
                } else {
                    top_set.erase(el);
                    top_val = top_val + top_gain;
                }
            }
        };
};