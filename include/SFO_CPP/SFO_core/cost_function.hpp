#pragma once
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <cmath>
#include "element.hpp"

namespace costfunction{
    class CostFunction{
        // Cost functions are given POINTERS to ELEMENTS, returning an evaluation
        public:
            virtual ~CostFunction(){};
            virtual double operator()(std::unordered_set<Element*> &set) = 0;
            virtual double operator()(Element* &el) = 0;
    };

    class Modular : public CostFunction{
        public:
            std::unordered_map<Element*, double> weights;
            
            Modular(const std::unordered_map<Element*, double> &w){
                weights = w;
            }

            Modular(const double &w){
                weights.insert({nullptr, w});
            }

            Modular(){
                weights.insert({nullptr, 1});
            }

            double operator()(std::unordered_set<Element*> &set){
                if (weights.size() == 1){
                    return (weights.begin()->second)*double(set.size());
                }
                double val = 0;
                for(auto el:set){
                    val = val + weights[el];
                }
                return val;
            }

            double operator()(Element* &el){
                if (weights.size() == 1){
                    return (weights.begin()->second);
                } else{
                    return weights[el];
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

            double operator()(std::unordered_set<Element*> &set){
                return std::sqrt(modular_part(set));
            }

            double operator()(Element* &el){
                return std::sqrt(modular_part(el));
            }
    };
}