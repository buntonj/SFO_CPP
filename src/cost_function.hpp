#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "element.hpp"
#pragma once

class CostFunction{
    virtual double operator()(std::unordered_set<Element> set) = 0;
    virtual double operator()(Element) = 0;
};

class Modular : CostFunction{
    std::unordered_map<int, double> weights;
    
    Modular(std::vector<double> w){
        for(int i = 0; i<w.size(); ++i){
            weights.insert({i, w[i]});
        }
    }

    Modular(){
        weights.insert({0, 1});
    }

    public:
        double operator()(std::unordered_set<Element*> set){
            if (weights.size() == 1){
                return (weights.begin()->second)*double(set.size());
            }
            double val = 0;
            for(auto el:set){
                val = val + weights[el->id];
            }
        }
    
};