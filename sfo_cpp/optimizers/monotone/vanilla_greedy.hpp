#pragma once
#include <iostream>
#include <unordered_set>
#include <cfloat>
#include "../../sfo_concepts/element.hpp"
#include "../../sfo_concepts/cost_function.hpp"
#include "../../sfo_concepts/constraint.hpp"

// WIP pared-down interface
// was allowing too many different instantiations/calls which was getting unclear/redundant/sloppy

template <typename E>
class VanillaGreedy
{
private:
    int MAXITER = 15;

public:
    double curr_val = 0; // current value of elements in set
    bool constraint_saturated = false;
    std::unordered_set<E *> *ground_set; // pointer to ground set of elements
    int n = 0;                           // holds size of ground set, indexed from 0 to n-1
    std::unordered_set<constraint::Constraint<E> *> constraint_set;
    costfunction::CostFunction<E> *cost_function;
    bool cost_benefit = false;
    std::unordered_set<E *> curr_set; // will hold elements selected to be in our set

    void set_ground_set(std::unordered_set<E *> *V)
    {
        this->ground_set = V;
        this->n = V->size();
    }

    void add_constraint(constraint::Constraint<E> *C)
    {
        this->constraint_set.insert(C);
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

    void set_cost_benefit(bool cb)
    {
        this->cost_benefit = cb;
    }

    void clear_set()
    {
        this->curr_set.clear();
        if (this->cost_function)
        {
            this->curr_val = this->cost_function->operator()(curr_set);
        }
        else
        {
            this->curr_val = 0;
        }
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
        else
        {
            return true;
        }
    }

    void run_greedy()
    {
        if (this->is_configured())
        {
            if (!(this->cost_benefit))
            {
                // if not asking for cost-benefit alg, run vanilla greedy
                int counter = 0;
                while (!constraint_saturated && counter < MAXITER)
                {
                    counter++;
                    greedy_step();
                    std::cout << "Performed VANILLA greedy algorithm iteration: " << counter << std::endl;
                    print_status();
                }
            }
            else if (constraint::Knapsack<E> *k = find_single_knapsack(); (k != nullptr))
            {
                // if asking for cost-benefit, check that constraint is a knapsack one
                // if it is, k becomes a pointer to derived Constraint::Knapsack type
                int counter = 0;
                double budget = 0;
                while (!constraint_saturated && counter < MAXITER)
                {
                    counter++;
                    cost_benefit_greedy_step(k, budget);
                    std::cout << "Performed VANILLA CB greedy algorithm iteration: " << counter << std::endl;
                    print_status();
                }
            }
            else
            {
                // if we were asking for CB but dynamically casting to a knapsack didn't work
                std::cout << "Requested CB greedy with invalid constraint type." << std::endl;
                return;
            }
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
    void greedy_step()
    {
        std::unordered_set<E *> test_set(this->curr_set);
        E *best_el;
        double best_marginal_val = -DBL_MAX;
        double candidate_marginal_val = 0;

        for (auto el = ground_set->begin(); el != ground_set->end(); ++el)
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

    void cost_benefit_greedy_step(constraint::Knapsack<E> *K, double &curr_budget)
    {
        std::unordered_set<E *> test_set(curr_set);
        E *best_el;
        double best_marginal_val = -DBL_MAX;
        double best_marginal_cost = 1;
        double candidate_marginal_cost = 1;
        double candidate_marginal_val = -DBL_MAX;

        for (auto el = ground_set->begin(); el != ground_set->end(); ++el)
        {
            // note that el is a POINTER to a POINTER to an element in the ground set
            test_set = curr_set;

            // if element is already in our set, skip it
            if (curr_set.find(*el) != curr_set.end())
            {
                continue;
            }

            test_set.insert(*el);

            // if new element violates the constraint, skip it
            if (!this->check_constraints(test_set))
            {
                continue;
            }

            // update marginal value
            candidate_marginal_cost = K->value(test_set) - curr_budget;
            candidate_marginal_val = cost_function->operator()(test_set) - curr_val;

            // keep running track of highest marginal value element
            if (candidate_marginal_val / candidate_marginal_cost > best_marginal_val / best_marginal_cost)
            {
                best_el = *el;
                best_marginal_val = candidate_marginal_val;
                best_marginal_cost = candidate_marginal_cost;
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

    constraint::Knapsack<E> *find_single_knapsack()
    {
        constraint::Knapsack<E> *knapsack_ptr;
        for (auto it = constraint_set.begin(); it != constraint_set.end(); ++it)
        {
            // iterate over constraints in set, looking for one that can be cast to knapsack
            knapsack_ptr = dynamic_cast<constraint::Knapsack<E> *>(*it);
            if (knapsack_ptr != nullptr)
            {
                // if we find one, return it (just the first one found)
                return knapsack_ptr;
            }
        }
        // if we didn't find one, then return nullptr
        return nullptr;
    }
};