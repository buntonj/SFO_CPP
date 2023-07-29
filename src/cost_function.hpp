#pragma once
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <cmath>
#include "element.hpp"

class CostFunction{
    public:
        virtual double operator()(std::unordered_set<Element> &set) = 0;
    // virtual double operator()(Element) = 0;
};

class Modular : public CostFunction{
    public:
        std::unordered_map<int, double> weights;
        
        Modular(std::vector<double> &w){
            for(int i = 0; i<w.size(); ++i){
                weights.insert({i, w[i]});
            }
        }

        Modular(){
            weights.insert({0, 1});
        }

        double operator()(std::unordered_set<Element> &set){
            if (weights.size() == 1){
                return (weights.begin()->second)*double(set.size());
            }
            double val = 0;
            for(auto el:set){
                val = val + weights[el.id];
            }
            return val;
        }

        double operator()(Element &el){
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

        SquareRootModular(Modular &m){
            modular_part = m;
        }

        SquareRootModular(){
            modular_part = Modular();
        }

        double operator()(std::unordered_set<Element> &set){
            return std::sqrt(modular_part(set));
        }

        double operator()(Element &el){
            return std::sqrt(modular_part(el));
        }

};