#pragma once
#include <iostream>
#include <unordered_set>
#include <vector>
#include <cfloat>
#include <algorithm> // for set difference
#include "../../sfo_concepts/element.hpp"
#include "../../sfo_concepts/cost_function.hpp"
#include "../../sfo_concepts/constraint.hpp"

template <typename E>
class ApxLocalSearch
{
private:
    int MAXITER = 15;
    double epsilon = 0;
    std::unordered_set<E *> curr_ground_set;

public:
    double curr_val = 0; // current value of elements in set
    bool constraint_saturated = false;
    std::unordered_set<E *> *ground_set; // pointer to ground set of elements
    int n = 0;                           // holds size of ground set, indexed from 0 to n-1
    std::unordered_set<constraint::Constraint<E> *> constraint_set;
    costfunction::CostFunction<E> *cost_function;
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

    void clear_set()
    {
        this->curr_set.clear();
        this->curr_val = 0;
        this->constraint_saturated = false;
    }

    void run_greedy()
    {
        if (this->n < 1)
        {
            std::cout << "No ground set given!" << std::endl;
            return;
        }

        // check if the constraint is compatible--cannot be knapsack
        if (constraint::Knapsack<E> *k = find_knapsack(); k != nullptr)
        {
            std::cout << "Constraint is a knapsack, apx local search is only valid with matroids." return;
        }

        if (this->epsilon <= 0)
        {
            std::cout << "Epsilon not set, using default value 0.25." << std::endl;
            this->epsilon = 0.25;
        }

        curr_ground_set = *ground_set; // initialize the ground set
        std::unordered_set<E *> candidate;
        double value = -DBL_MAX;
        double test_value;
        for (int i = 0; i < constraint_set.size(); i++)
        {
            // for each constraint, run a local search, save values
            candidate = local_search_procedure();
            test_value = cost_function->evaluate(candidate);

            // check for new maximum, if we found it save it
            if (test_value > value)
            {
                value = test_value;
                curr_set = candidate;
            }

            // remove the selected elements from the ground set
            curr_ground_set = std::set_difference(curr_ground_set.begin(), curr_ground_set.end(),
                                                  candidate.begin(), candidate.end(),
                                                  std::inserter(curr_ground_set, curr_ground_set.end()));
        }
    }

    void print_status()
    {
        std::cout << "Current set:";
        std::cout << curr_set;
        std::cout << "Current val: " << curr_val << std::endl;
        std::cout << "Constraint saturated? " << constraint_saturated << std::endl;
    };

private:
    // TODO: This should return a subset candidate
    void local_search_procedure()
    {
        double curr_val = DBL_MIN;
        double test_val;
        std::unordered_set<E *> test_set;
        E *test_el;

        // find max value singleton element
        for (auto it = curr_ground_set.begin(); it != curr_ground_set.end(); ++it)
        {
            if (this->check_constraints(*it))
            {
                // if element is feasible, compute its value
                test_val = cost_function->evaluate(*it);

                // save it if it is better than current best
                if (test_val > curr_val)
                {
                    test_el = *it;
                    curr_val = test_val;
                }
            }
        }

        // test_el now holds the max value element in the ground set
        std::unordered_set<E *> curr_set({test_el});

        // Start with exchange and delete assumed impossible
        bool successful_exchange = true;
        bool successful_delete = true;

        double gain_threshold = 1.0 + epsilon / pow(double(curr_ground_set.size()), 4.0);

        while (successful_exchange || successful_delete)
        {
            // Indicate no successful ops have happened
            successful_exchange = false;
            successful_delete = false;

            // Check ability to delete element from current set while increasing value
            test_set = curr_set;
            for (auto el = curr_set.begin(); el != curr_set.end(); ++el)
            {
                test_set.remove(el);
                test_val = cost_function->evaluate(test_set);

                // If we find a valuable delete operation, use it immediately
                if (test_val >= gain_threshold * curr_val)
                {
                    // Swap to the new set and value
                    curr_set = test_set;
                    curr_val = test_val;
                    // Indicate we successfully deleted an element, break from trying to do so
                    successful_delete = true;
                    break
                }
            }

            // check ability to exchange element from set while increasing value
            // Loop over all elements in ground set
            // Look for UP TO size(constraint_set) elements to remove (one if one matroid)
            for (auto el = curr_ground_set.begin(); el != curr_ground_set.end(); ++el)
            {
                test_set = curr_set;

                // grab the element, check feasibility
                test_set.insert(el);
                if
                    ~check_constraints(test_set)
                    {
                        // If infeasible, there must be an subset in the k-neighborhood that is feasible
                        // need to search k-neighborhood of curr_set,
                        // i.e., all size <=k subsets of curr_set that might cause curr_set + el - subset to be feasible
                        // likely a recursive function here
                        test_val = best_feasible_removable_count(test_set, size(constraint_set));
                        // need to also return the subset
                    }
                else
                {
                    // element can be feasibly added
                    test_val = cost_function->evaluate(test_set);
                }

                if (test_val >= gain_threshold * curr_val)
                {
                    // if sufficient gain, just do the exchange immediately and go to next apx local search iteration
                    curr_set = test_set;
                    curr_val = test_val;
                    successful_exchange = true;
                    break
                }
            }
        }
    }

    double best_feasible_removable_count(std::unordered_set<E *> test_set, size_t remove_budget)
    {
        double curr_value = check_constraints(test_set) ? cost_function->evaluate(test_set) : DBL_MIN;
        if ((remove_budget == 0))
        {
            // Base case: we can't remove any more elements
            return curr_value;
        }
        else
        {
            // if we can remove more elements
            // iterate over remaining elements trying to remove em
            std::unordered_set<E *> new_set = test_set;
            for (auto el : test_set.begin(); el != test_set.end(); ++el)
            {
                // try removing this element, see what the best value is
                new_set = test_set;
                new_set.remove(el);
                curr_value = max(curr_value, best_feasible_removable_count(new_set, remove_budget - 1));
            }
            // Return best value we found from our exchange operation hunt
            return curr_value;
        }
    }

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