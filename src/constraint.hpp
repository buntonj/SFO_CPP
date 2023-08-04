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
        virtual bool test_membership(std::unordered_set<Element> &set){
            return true;
        }

        virtual bool test_membership(Element &el){
            return true;
        }

        virtual bool is_saturated(Element &el){
            return false;
        }

        virtual bool is_saturated(std::unordered_set<Element> &set){
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

        bool test_membership(Element &el){
            return modular(el) <= budget;
        }

        bool test_membership(std::unordered_set<Element> &set){
            return modular(set) <= budget;
        }

        bool is_saturated(std::unordered_set<Element> &test_set){
            return std::abs(modular(test_set) - budget) < __FLT_EPSILON__;
        }

        bool is_saturated(Element &el){
            return std::abs(modular(el) - budget) < __FLT_EPSILON__;
        }
};