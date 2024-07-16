#pragma once
#include <unordered_set>
#include <iostream>
#include <vector>
#include <cfloat>
#include "../../sfo_concepts/element.hpp"
#include "../../sfo_concepts/cost_function.hpp"
#include "../../sfo_concepts/constraint.hpp"

template <typename E>
class StochasticGreedy
{
private:
    int b;
    int MAXITER = 15;
    std::vector<E *> ground_set_idxs; // this maps us from an integer to an element
    std::unordered_set<E *> to_erase; // used to discard and no longer randomly sample elements

public:
    double curr_val = 0; // current value of elements in set
    bool constraint_saturated = false;
    std::unordered_set<E *> *ground_set;
    int n = 0; // holds size of ground set, indexed from 0 to n-1
    std::unordered_set<constraint::Constraint<E> *> constraint_set;
    costfunction::CostFunction<E> *cost_function;
    std::unordered_set<E *> curr_set; // will hold elements selected to be in our set
    double epsilon = 0;

    void set_ground_set(std::unordered_set<E *> *V)
    {
        this->ground_set = V;
        this->n = V->size();
        this->index_ground_set();
    }

    void add_constraint(constraint::Constraint<E> *C)
    {
        if (constraint::Cardinality<E> *k = dynamic_cast<constraint::Cardinality<E> *>(C); k != nullptr)
        {
            this->constraint_set.insert(C);
        }
        else
        {
            std::cout << "Stochastic greedy is only valid with cardinality constraints." << std::endl;
        }
    }

    void remove_constraint(constraint::Constraint<E> *C)
    {
        this->constraint_set.erase(C);
    }

    bool check_constraints(std::unordered_set<E *> set)
    {
        for (auto iter = constraint_set.begin(); iter != constraint_set.end(); ++iter)
        {
            if (!((*iter)->test_membership(set)))
            {
                // if any constraint is not satisfied, then intersection of them is not
                return false;
            }
        }
        return true; // if all constraints were satisfied, then return true
    }

    bool check_saturated(std::unordered_set<E *> set)
    {
        for (auto iter = constraint_set.begin(); iter != constraint_set.end(); ++iter)
        {
            if ((*iter)->is_saturated(set))
            {
                // if any constraint is saturated, then so is intersection
                return true;
            }
        }
        return false; // if no constraints were saturated, then return true
    }

    void set_cost_function(costfunction::CostFunction<E> *F)
    {
        this->cost_function = F;
    }

    void set_epsilon(double epsilon)
    {
        this->epsilon = epsilon;
    }

    void clear_set()
    {
        this->curr_set.clear();
        this->curr_val = 0;
        this->constraint_saturated = false;
    }

    bool is_configured()
    {
        if (!this->ground_set)
        {
            std::cout << "No ground set given!" << std::endl;
            return false;
        }
        else if (!this->cost_function)
        {
            std::cout << "No cost function given!" << std::endl;
            return false;
        }
        else if (!(find_single_cardinality() && constraint_set.size() == 1))
        {
            std::cout << "Constraint is not a single cardinality constraint, stochastic greedy is not valid." << std::endl;
            return false;
        }
        else
        {
            return true;
        }
    }

    void run_greedy()
    {
        if (this->is_configured())
        {
            if (epsilon <= 0)
            {
                std::cout << "Epsilon value not set/valid, using default 0.1..." << std::endl;
                this->epsilon = 0.1;
            }
            // stochastic greedy is only valid for cardinality constraints, so check
            constraint::Cardinality<E> *k = find_single_cardinality();
            this->b = k->budget;
            this->curr_val = 0;
            // first, compute how many samples to randomly pull at each step
            int sample_size = compute_random_set_size();
            std::unordered_set<E *> *sample_set = new std::unordered_set<E *>;
            int counter = 0;
            while (!constraint_saturated && counter < MAXITER)
            {
                counter++;
                *sample_set = sample_ground_set(sample_size);
                std::cout << "Sampled set: " << *sample_set << std::endl;
                stochastic_greedy_step(sample_set);
                sample_size = std::min(sample_size, int(ground_set_idxs.size()));
                std::cout << "Performed greedy algorithm iteration: " << counter << std::endl;
                print_status();
            }
        }
        else
        {
            return;
        }
    };

    void print_status()
    {
        std::cout << "Current set:";
        std::cout << curr_set;
        std::cout << "Current val: " << curr_val << std::endl;
        std::cout << "Constraint saturated? " << constraint_saturated << std::endl;
    };

private:
    int compute_random_set_size()
    {
        return int(std::min((double(this->n) / this->b) * log(1.0 / this->epsilon), double(this->n)));
    }

    void index_ground_set()
    {
        int idx = 0;
        for (auto el = ground_set->begin(); el != ground_set->end(); ++el, ++idx)
        {
            ground_set_idxs.push_back(*el);
        }
    }

    std::unordered_set<E *> sample_ground_set(int &set_size)
    {
        std::unordered_set<E *> random_set;
        E *candidate;
        int rand_idx;
        int count = 0;
        // main sampling loop
        while (count < set_size)
        {
            rand_idx = rand() % ground_set_idxs.size(); // pull a random index between 0 and n
            candidate = ground_set_idxs[rand_idx];      // find which element pointer it corresponds to

            // first, check if it is no longer feasible and toss it if so
            if (to_erase.find(candidate) != to_erase.end())
            {
                ground_set_idxs.erase(ground_set_idxs.begin() + rand_idx);
                to_erase.erase(candidate);
            }
            else
            {
                // then see if we haven't added it to the set already
                if (random_set.find(candidate) == random_set.end())
                {
                    random_set.insert(ground_set_idxs[rand_idx]);
                    count++; // increment number of elements in our set
                }
                else
                {
                    continue;
                }
            }
        }
        return random_set;
    }

    void stochastic_greedy_step(std::unordered_set<E *> *sampled_set)
    {
        std::unordered_set<E *> test_set(curr_set);
        E *best_el;
        double best_marginal_val = -DBL_MAX;
        double candidate_marginal_val = 0;

        // randomly sample a set of size sample_size
        // compute the marginal gains for the elements in that set
        // choose the max gain element from the set

        for (auto el = sampled_set->begin(); el != sampled_set->end(); ++el)
        {
            // note that el is a POINTER to a POINTER to an element in the ground set
            test_set = curr_set;

            if (curr_set.find(*el) != curr_set.end())
            {
                continue;
            }

            test_set.insert(*el);

            if (!this->check_constraints(test_set))
            {
                // mark the element to not be considered or sampled
                this->to_erase.insert(*el);
                continue;
            }

            // update marginal value
            candidate_marginal_val = cost_function->operator()(test_set) - curr_val;

            // keep running track of highest marginal value element
            if (candidate_marginal_val > best_marginal_val)
            {
                best_el = *el;
                best_marginal_val = candidate_marginal_val;
            }
        }

        // check if we could even add an element to set
        if (best_marginal_val < 0)
        {
            constraint_saturated = true; // no more elements could be feasibly added
        }
        else
        {
            // update the current set, value, and budget value with the found item
            curr_set.insert(best_el);
            curr_val = curr_val + best_marginal_val;
            constraint_saturated = this->check_saturated(curr_set); // check if constraint is now saturated
        }
    };

    constraint::Cardinality<E> *find_single_cardinality()
    {
        constraint::Cardinality<E> *cardinality_ptr;
        for (auto it = constraint_set.begin(); it != constraint_set.end(); ++it)
        {
            // iterate over constraints in set, looking for one that can be cast to knapsack
            cardinality_ptr = dynamic_cast<constraint::Cardinality<E> *>(*it);
            if (cardinality_ptr != nullptr)
            {
                // if we find the only one, return it
                return cardinality_ptr;
            }
        }
        // if we didn't find one unique constraint, then return nullptr
        return nullptr;
    }
};