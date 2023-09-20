#pragma once
#include <queue>
#include <unordered_set>
#include <set>
#include <iostream>
#include <vector>
#include <cfloat>
#include "../../sfo_concepts/element.hpp"
#include "../../sfo_concepts/cost_function.hpp"
#include "../../sfo_concepts/constraint.hpp"

template<typename E> class LazierThanLazyGreedy{
    private:
        double curr_val = 0;  // current value of elements in set
        int b;
        bool constraint_saturated = false;
        int MAXITER = 15;

    public:
        int n = 0;  // holds size of ground set, indexed from 0 to n-1
        constraint::Constraint<E> *constraint;
        std::unordered_set<E*> *ground_set;
        std::vector<E*> ground_set_idxs;  // this maps us from an integer to an element
        std::unordered_set<E*> curr_set;  // will hold elements selected to be in our set
        std::unordered_map<E*, double> marginals;  // will hold marginal values of all elements we have evaluated

        LazierThanLazyGreedy(int &N){
            this->set_ground_set(this->generate_ground_set(N));
        };

        LazierThanLazyGreedy(int &N, int &B){  // If you give a budget, initialize a budget constraint
            this->set_ground_set(this->generate_ground_set(N));
            this->add_constraint(new constraint::Cardinality<E>(B));
        };

        LazierThanLazyGreedy(std::unordered_set<E*> *V){
            this->set_ground_set(V);
        };

        LazierThanLazyGreedy(std::unordered_set<E*> *V, int &B){
            this->set_ground_set(V);
            this->add_constraint(new constraint::Cardinality<E>(B));
        };

        std::unordered_set<E*>* generate_ground_set(int &n){
            std::unordered_set<E*> *V = new std::unordered_set<E*>;
            E* el;
            int id = 0;
            for (int i=0; i<n; i++){
                id++;
                el = new E;
                el->id = id;
                el->value = 0;
                V->insert(el);
            }
            return V;
        }

        void set_ground_set(std::unordered_set<E*> *V){
            this->ground_set = V;
            this->n = V->size();
            this->index_ground_set();
        }

        void index_ground_set(){
            int idx = 0;
            for (auto el=ground_set->begin(); el != ground_set->end(); ++el, ++idx){
                ground_set_idxs.push_back(*el);
                marginals.insert({*el, DBL_MAX});
            }
        }

        LazyGreedyQueue<E> sample_to_marginals(std::unordered_set<E*> *sample_set){
            LazyGreedyQueue<E> sample_marginals;
            std::pair<E*, double> marginal;
            for(auto it:*sample_set){
                marginal.first = it;
                if(marginals.find(it) != marginals.end()){
                    marginal.second = marginals[it];
                } else {
                    marginal.second = DBL_MAX;
                }
                sample_marginals.push(marginal);
            }
            return sample_marginals;
        }

        void update_marginals(LazyGreedyQueue<E> sampled_marginals){
            // helper function
            // updates the running total marginals with the sampled set
            std::pair<E*, double> candidate;
            while(!sampled_marginals.empty()){
                candidate = sampled_marginals.top();
                marginals[candidate.first] = candidate.second;
                sampled_marginals.pop();
            }
        }

        void reset_marginals(){
            this->index_ground_set();
        }

        void clear_set(){
            this->curr_set.clear();
            this->curr_val = 0;
            this->constraint_saturated = false;
            this->reset_marginals();
        }

        void run_greedy(costfunction::CostFunction<E> &C, double epsilon){
            // stochastic greedy is only valid for cardinality constraints, so check
            if (constraint::Cardinality<E>* k = dynamic_cast<constraint::Cardinality<E>*>(constraint); k != nullptr){
                this->b = k->budget;
                this->curr_val = 0;
                // first, compute how many samples to randomly pull at each step
                int sample_size = random_set_size(epsilon, n, double(b));
                std::unordered_set<E*> *sample_set = new std::unordered_set<E*>;
                LazyGreedyQueue<E> sample_marginals;
                int counter=0;
                while (!constraint_saturated && counter < MAXITER){
                    counter++;
                    *sample_set = sample_ground_set(sample_size);  // we need to sample the valid marginals now, not full ground set
                    std::cout<<"Sampled set: "<< *sample_set <<std::endl;
                    sample_marginals = sample_to_marginals(sample_set);
                    lazier_than_lazy_greedy_step(C, sample_marginals);
                    update_marginals(sample_marginals);
                    sample_size = std::min(sample_size, int(marginals.size()));
                    std::cout<< "Performed greedy algorithm iteration: " << counter << std::endl;
                    print_status();
                }
            } else {
                std::cout<<"Constraint is not a cardinality constraint, stochastic greedy is not valid."<<std::endl;
            }
        };

        void print_status(){
            std::cout << "Current set:";
            std::cout<< curr_set;
            std::cout<<"Current val: " << curr_val << std::endl;
            std::cout<<"Constraint saturated? " << constraint_saturated << std::endl;
        };

        void add_constraint(constraint::Constraint<E> *C){
            constraint = C;
        }

    private:
        int random_set_size(double epsilon, int ground_set_size, double budget){
            return int(std::min((double(ground_set_size)/budget)*log(1.0/epsilon), double(ground_set_size)));
        }

        std::unordered_set<E*> sample_ground_set(int &set_size){
            std::unordered_set<E*> random_set;
            E* candidate;
            int rand_idx;
            int count = 0;
            // main sampling loop
            while(count  < set_size){
                rand_idx = rand() % ground_set_idxs.size();  // pull a random index between 0 and n
                candidate = ground_set_idxs[rand_idx]; // find which element pointer it corresponds to
                // first see if we have added it to the set already
                if(random_set.find(candidate) == random_set.end()){
                    if (marginals.find(candidate) == marginals.end()){
                        // if there is no associated marginal, then it is no longer feasible
                        ground_set_idxs.erase(ground_set_idxs.begin()+rand_idx);
                    } else{
                        random_set.insert(ground_set_idxs[rand_idx]);
                        count++; // increment number of elements in our set
                    }
                } else {
                    continue;
                }
            }
            return random_set;
        }

        void lazier_than_lazy_greedy_step(costfunction::CostFunction<E> &F, LazyGreedyQueue<E> &sampled_marginals){
            std::unordered_set <E*> test_set(curr_set);
            std::pair<E*, double> candidate;

            // iterate through all candidate element IDs
            while(! sampled_marginals.empty()){
                test_set = curr_set;

                // pull first element from priority queue
                candidate.first = sampled_marginals.top().first;
                test_set.insert(candidate.first);  // add it to testing set

                if (!constraint->test_membership(test_set)){
                    sampled_marginals.pop();
                    marginals.erase(candidate.first);  // leave that element out from now on
                    continue;
                }

                candidate.second = F(test_set)-curr_val;

                // put updated candidate back into priority queue
                sampled_marginals.pop();
                sampled_marginals.push(candidate);

                // if it is still at the top, we have found best element
                if (sampled_marginals.top().second <= candidate.second){
                    break;
                }
            }

            if(! sampled_marginals.empty()){
                if (auto best = sampled_marginals.top(); best.second > 0){
                    // update the current set, value, and budget value with the found item
                    curr_set.insert(best.first);
                    curr_val = curr_val + best.second;
                    sampled_marginals.pop();
                    constraint_saturated = constraint->is_saturated(curr_set); // allows for early stop detection
                } else {
                    if(marginals.empty()){
                        constraint_saturated = true;
                    }
                }
            } else{
                // we are really only at feasible limit if we have nothing left to sample
                if(marginals.empty()){
                    constraint_saturated = true;
                }
            }
        };
};