#pragma once
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <cmath>
#include "element.hpp"

namespace costfunction{
    template<typename E> class CostFunction{
        // Cost functions are given POINTERS to ELEMENTS, returning an evaluation
        public:
            virtual ~CostFunction(){};
            virtual double operator()(std::unordered_set<E*> &set) = 0;
            virtual double operator()(E* &el) = 0;
    };

    template<typename E>class Modular : public CostFunction<E>{
        public:
            std::unordered_map<E*, double> weights;
            
            Modular(const std::unordered_map<E*, double> &w){
                weights = w;
            }

            Modular(const double &w){
                weights.insert({nullptr, w});
            }

            Modular(){
                weights.insert({nullptr, 1});
            }

            double operator()(std::unordered_set<E*> &set){
                if (weights.size() == 1){
                    return (weights.begin()->second)*double(set.size());
                }
                double val = 0;
                for(auto el:set){
                    val = val + weights[el];
                }
                return val;
            }

            double operator()(E* &el){
                if (weights.size() == 1){
                    return (weights.begin()->second);
                } else{
                    return weights[el];
                }
            }
        
    };

    template<typename E> class SquareRootModular: public CostFunction<E>{
        public:
            Modular<E> modular_part;

            SquareRootModular(const Modular<E> &m){
                modular_part = m;
            }

            SquareRootModular(){
                modular_part = Modular<E>();
            }

            double operator()(std::unordered_set<E*> &set){
                return std::sqrt(modular_part(set));
            }

            double operator()(E* &el){
                return std::sqrt(modular_part(el));
            }
    };
}