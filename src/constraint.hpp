#pragma once
#include<unordered_map>
#include "cost_function.hpp"
#include<unordered_set>
#include<iostream>
#include "element.hpp"

class Constraint{
    public:
        Constraint(){};
    
        // No constraint defined at high level, so just always return true.
        virtual const bool test_membership(std::unordered_set<Element> &set){
            return true;
        }

        virtual const bool test_membership(Element &el){
            return true;
        }

        virtual const bool is_saturated(Element &el){
            return false;
        }

        virtual const bool is_saturated(std::unordered_set<Element> &set){
            return false;
        }
};


class Knapsack : public Constraint{
    // knapsack constraints are secretly just Modular functions with an upper bound
    public:
        Modular modular;
        double budget;

        template<typename T>
        Knapsack(const T &wts, const double &B){
            modular = Modular(wts);
            budget = B;
        }

        Knapsack(const double &w, const double &B){
            modular = Modular(w);
            budget = B;
        }
        
        // this is just the cardinality constraint oracle
        Knapsack(const double &B){
            modular = Modular();
            budget = B;
        }

        const bool test_membership(const Element &el){
            return modular(el) <= budget;
        }

        const bool test_membership(std::unordered_set<Element> &set){
            return modular(set) <= budget;
        }

        const bool is_saturated(std::unordered_set<Element> &test_set){
            return std::abs(modular(test_set) - budget) < __FLT_EPSILON__;
        }

        const bool is_saturated(Element &el){
            return std::abs(modular(el) - budget) < __FLT_EPSILON__;
        }

        const double value(std::unordered_set<Element> &test_set){
            return modular(test_set);
        }

        const double value(Element &el){
            return modular(el);
        }
};