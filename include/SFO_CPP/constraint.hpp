#pragma once
#include<unordered_map>
#include "cost_function.hpp"
#include<unordered_set>
#include<iostream>
#include "element.hpp"

namespace constraint{
    class Constraint{
        public:
            Constraint(){};

            virtual ~Constraint(){};

            // No constraint defined at high level, so just always return true.
            virtual bool test_membership(std::unordered_set<Element*>&){
                return true;
            }

            virtual bool test_membership(Element*){
                return true;
            }

            virtual bool is_saturated(Element*){
                return false;
            }

            virtual bool is_saturated(std::unordered_set<Element*>&){
                return false;
            }
    };


    class Knapsack : public Constraint{
        // knapsack constraints are secretly just Modular functions with an upper bound
        public:
            costfunction::Modular modular;
            double budget;

            template<typename T>
            Knapsack(const T &wts, const double &B){
                modular = costfunction::Modular(wts);
                budget = B;
            }
            
            // this is just the cardinality constraint oracle
            Knapsack(const double &B){
                modular = costfunction::Modular();
                budget = B;
            }

            bool test_membership(Element *el){
                return modular(el) <= budget;
            }

            bool test_membership(std::unordered_set<Element*> &set){
                return modular(set) <= budget;
            }

            bool is_saturated(std::unordered_set<Element*> &test_set){
                return std::abs(modular(test_set) - budget) < __FLT_EPSILON__;
            }

            bool is_saturated(Element *el){
                return std::abs(modular(el) - budget) < __FLT_EPSILON__;
            }

            double value(std::unordered_set<Element*> &test_set){
                return modular(test_set);
            }

            double value(Element* el){
                return modular(el);
            }
    };

    class Cardinality : public Knapsack{
        // cardinality constraints are just a special case of modular/knapsack constraints
        public:
            Cardinality(const int &B) : Knapsack(int(B)){}
    };
}