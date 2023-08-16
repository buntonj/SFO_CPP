#pragma once
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <cmath>
#include "element.hpp"

class CostFunction{
    public:
        const virtual double operator()(const std::unordered_set<Element> &set) = 0;
    // virtual double operator()(Element) = 0;
};

class Modular : public CostFunction{
    public:
        std::unordered_map<int, double> weights;
        
        Modular(const std::vector<double> &w){
            for(int i = 0; i<w.size(); ++i){
                weights.insert({i, w[i]});
            }
        }

        Modular(const double &w){
            weights.insert({0, w});
        }

        Modular(){
            weights.insert({0, 1});
        }

        const double operator()(const std::unordered_set<Element> &set){
            if (weights.size() == 1){
                return (weights.begin()->second)*double(set.size());
            }
            double val = 0;
            for(auto el:set){
                val = val + weights[el.id];
            }
            return val;
        }

        const double operator()(const Element &el){
            if (weights.size() == 1){
                return (weights.begin()->second);
            } else{
                return weights[el.id];
            }
        }
    
};

class SquareRootModular: public CostFunction{
    public:
        Modular modular_part;

        SquareRootModular(const Modular &m){
            modular_part = m;
        }

        SquareRootModular(){
            modular_part = Modular();
        }

        const double operator()(const std::unordered_set<Element> &set){
            return std::sqrt(modular_part(set));
        }

        const double operator()(const Element &el){
            return std::sqrt(modular_part(el));
        }

};