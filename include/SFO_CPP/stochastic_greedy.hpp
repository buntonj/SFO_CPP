#pragma once
#include <queue>
#include <unordered_set>
#include <iostream>
#include <vector>
#include <cfloat>
#include "SFO_core/element.hpp"
#include "SFO_core/cost_function.hpp"
#include "SFO_core/constraint.hpp"


// need to map numbers from 1-N to element*'s in the ground set
// need to track elements in container sorted by marginal_value
// need to be able to access subsets of the container according to random sampling
// set sorts by element comparison operator (value, Element*)

// comparison function for sorting elements by marginal value
/*class compare_elements{
    public:
        bool operator()(const std::pair<Element*, double> &lhs, const std::pair<Element*, double> &rhs){
            return lhs.second < rhs.second;
        };
};

// for brevity later, defines a priority queue where element pointers are sorted by marginal value
typedef std::priority_queue<std::pair<Element*, double>,std::vector<std::pair<Element*,double>>,compare_elements> LazyGreedyQueue;
*/
class StochasticGreedyAlgorithm{
    private:
        double curr_val = 0;  // current value of elements in set
        int b;
        bool constraint_saturated = false;
        int MAXITER = 15;

    public:
        int n = 0;  // holds size of ground set, indexed from 0 to n-1
        constraint::Constraint *constraint;
        std::unordered_set<Element*> *ground_set;
        std::unordered_map<int, Element*> ground_set_idxs;  // this maps us from an integer to an element
        std::unordered_set<Element*> curr_set;  // will hold elements selected to be in our set

        StochasticGreedyAlgorithm(int &N){
            this->set_ground_set(this->generate_ground_set(N));
        };

        StochasticGreedyAlgorithm(int &N, int &B){  // If you give a budget, initialize a budget constraint
            this->set_ground_set(this->generate_ground_set(N));
            this->add_constraint(new constraint::Cardinality(B));
        };

        StochasticGreedyAlgorithm(std::unordered_set<Element*> *V){
            this->set_ground_set(V);
        };

        StochasticGreedyAlgorithm(std::unordered_set<Element*> *V, int &B){
            this->set_ground_set(V);
            this->add_constraint(new constraint::Cardinality(B));
        };

        std::unordered_set<Element*>* generate_ground_set(int &n){
            std::unordered_set<Element*> *V = new std::unordered_set<Element*>;
            Element* el;
            int id = 0;
            for (int i=0; i<n; i++){
                id++;
                el = new Element;
                el->id = id;
                el->value = 0;
                V->insert(el);
            }
            return V;
        }

        void set_ground_set(std::unordered_set<Element*> *V){
            this->ground_set = V;
            this->n = V->size();
            this->index_ground_set();
        }

        void index_ground_set(){
            int idx = 0;
            for (auto el=ground_set->begin(); el != ground_set->end(); ++el, ++idx){
                ground_set_idxs.insert({idx, *el});
            }
        }

        void clear_set(){
            this->curr_set.clear();
            this->curr_val = 0;
            this->constraint_saturated = false;
        }

        void run_greedy(costfunction::CostFunction &C, double epsilon){
            // stochastic greedy is only valid for cardinality constraints, so check
            if (constraint::Cardinality* k = dynamic_cast<constraint::Cardinality*>(constraint); k != nullptr){

                this->curr_val = 0;
                // first, compute how many samples to randomly pull at each step
                int sample_size = random_set_size(epsilon, n, double(b));
                std::unordered_set<Element*> *sample_set = new std::unordered_set<Element*>;
                int counter=0;
                while (!constraint_saturated && counter < MAXITER){
                    counter++;
                    *sample_set = sample_ground_set(sample_size);
                    stochastic_greedy_step(C, sample_set);
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

        void add_constraint(constraint::Constraint *C){
            constraint = C;
        }

    private:
        int random_set_size(double epsilon, int ground_set_size, double budget){
            return int(std::min((double(ground_set_size)/budget)*log(1.0/epsilon), double(ground_set_size)));
        }

        std::unordered_set<Element*> sample_ground_set(int &set_size){
            std::unordered_set<Element*> random_set;
            Element* candidate;
            int rand_idx;
            int count = 0;
            // main sampling loop
            while(count  < set_size){
                rand_idx = rand() % n;  // pull a random index between 0 and n
                candidate = ground_set_idxs[rand_idx]; // find which element pointer it corresponds to

                // first see if we have added it to the set already
                if(random_set.find(candidate) == random_set.end()){
                    random_set.insert(ground_set_idxs[rand_idx]);
                    count++; // increment number of elements in our set
                } else {
                    continue;
                }
            }
            return random_set;
        }

        void stochastic_greedy_step(costfunction::CostFunction &F, std::unordered_set<Element*> *sampled_set){
            std::unordered_set <Element*> test_set(curr_set);
            Element* best_el;
            double best_marginal_val = -DBL_MAX;
            double candidate_marginal_val = 0;

            // randomly sample a set of size sample_size
            // compute the marginal gains for the elements in that set
            // choose the max gain element from the set

            for(auto el=sampled_set->begin(); el != sampled_set->end(); ++el){
                // note that el is a POINTER to a POINTER to an element in the ground set
                test_set = curr_set;

                if (curr_set.find(*el) != curr_set.end()){
                    continue;
                }

                test_set.insert(*el);

                if (! constraint->test_membership(test_set)){
                    continue;
                }

                // update marginal value
                candidate_marginal_val = F(test_set) - curr_val;

                // keep running track of highest marginal value element
                if (candidate_marginal_val > best_marginal_val){
                    best_el = *el;
                    best_marginal_val = candidate_marginal_val;
                }
            }

            // check if we could even add an element to set
            if (best_marginal_val < 0){
                constraint_saturated = true;  // no more elements could be feasibly added
            } else{
                // update the current set, value, and budget value with the found item
                curr_set.insert(best_el);
                curr_val = curr_val + best_marginal_val;
                constraint_saturated = constraint->is_saturated(curr_set);  // check if constraint is now saturated
            }
        };
};