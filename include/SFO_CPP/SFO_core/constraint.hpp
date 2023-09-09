#pragma once
#include<unordered_map>
#include "cost_function.hpp"
#include<unordered_set>
#include<iostream>
#include "element.hpp"

namespace constraint{
    template<typename E> class Constraint{
        public:
            Constraint(){};

            virtual ~Constraint(){};

            // No constraint defined at high level, so just always return true.
            virtual bool test_membership(std::unordered_set<E*>&){
                return true;
            }

            virtual bool test_membership(E*){
                return true;
            }

            virtual bool is_saturated(E*){
                return false;
            }

            virtual bool is_saturated(std::unordered_set<E*>&){
                return false;
            }
    };


    template<typename E> class Knapsack : public Constraint<E>{
        // knapsack constraints are secretly just Modular functions with an upper bound
        public:
            costfunction::Modular<E> modular;
            double budget;

            template<typename T>
            Knapsack(const T &wts, const double &B){
                modular = costfunction::Modular<E>(wts);
                budget = B;
            }
            
            // this is just the cardinality constraint oracle
            Knapsack(const double &B){
                modular = costfunction::Modular<E>();
                budget = B;
            }

            bool test_membership(E *el){
                return modular(el) <= budget;
            }

            bool test_membership(std::unordered_set<E*> &set){
                return modular(set) <= budget;
            }

            bool is_saturated(std::unordered_set<E*> &test_set){
                return std::abs(modular(test_set) - budget) < __FLT_EPSILON__;
            }

            bool is_saturated(E *el){
                return std::abs(modular(el) - budget) < __FLT_EPSILON__;
            }

            double value(std::unordered_set<E*> &test_set){
                return modular(test_set);
            }

            double value(E* el){
                return modular(el);
            }
    };

    template<typename E> class Cardinality : public Knapsack<E>{
        // cardinality constraints are just a special case of modular/knapsack constraints
        public:
            Cardinality(const int &B) : Knapsack<E>(int(B)){}
    };
}