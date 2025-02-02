#pragma once
#include <unordered_map>
#include "cost_function.hpp"
#include <unordered_set>
#include <vector>
#include <iostream>
#include "element.hpp"

namespace constraint
{
    template <typename E>
    class Constraint
    {
    public:
        Constraint() {};

        virtual ~Constraint() {};

        // No constraint defined at high level, so just always return true.
        virtual bool test_membership(std::unordered_set<E *> &)
        {
            return true;
        }

        virtual bool test_membership(E *)
        {
            return true;
        }

        virtual bool is_saturated(E *)
        {
            return false;
        }

        virtual bool is_saturated(std::unordered_set<E *> &)
        {
            return false;
        }
    };

    template <typename E>
    class Knapsack : public Constraint<E>
    {
        // knapsack constraints are secretly just Modular functions with an upper bound
    public:
        costfunction::Modular<E> modular;
        double budget;

        template <typename T>
        Knapsack(const T &wts, const double &B)
        {
            modular = costfunction::Modular<E>(wts);
            budget = B;
        }

        // this is just the cardinality constraint oracle
        Knapsack(const double &B)
        {
            modular = costfunction::Modular<E>();
            budget = B;
        }

        bool test_membership(E *el)
        {
            return modular.evaluate(el) <= budget;
        }

        bool test_membership(std::unordered_set<E *> &set)
        {
            return modular.evaluate(set) <= budget;
        }

        bool is_saturated(std::unordered_set<E *> &test_set)
        {
            return std::abs(modular.evaluate(test_set) - budget) < std::numeric_limits<float>::epsilon();
        }

        bool is_saturated(E *el)
        {
            return std::abs(modular.evaluate(el) - budget) < std::numeric_limits<float>::epsilon();
        }

        double value(std::unordered_set<E *> &test_set)
        {
            return modular.evaluate(test_set);
        }

        double value(E *el)
        {
            return modular.evaluate(el);
        }
    };

    template <typename E>
    class Cardinality : public Knapsack<E>
    {
        // cardinality constraints are just a special case of modular/knapsack constraints
    public:
        Cardinality(const int &B) : Knapsack<E>(int(B)) {}
    };

    template <typename E>
    class PartitionMatroid : public Constraint<E>
    {
        // Partition matroids consist of:
        //  * a division of the ground set into disjoint groups
        //  * a limit on how many elements of each group is valid
    public:
        // Partition is defined as a map from an element to a size_t identifier of which group
        // the element belongs to
        std::unordered_map<E *, size_t> partition;
        // The budget for each partition is held as a vector
        std::vector<int> budget;

        bool test_membership(E *el)
        {
            // Find the partition that this element is in
            size_t part = partition[el];
            // Check if the budget is high enough to use the element
            return budget[part] <= 1;
        }

        bool test_membership(std::unordered_set<E *> &set)
        {
            // Make a copy of the budget to evaluate with
            std::vector<int> test_budget = budget;
            size_t part;
            for (auto el : set)
            {
                part = partition[el];
                test_budget[part] -= 1;

                if (test_budget[part] < 0)
                {
                    // Fail fast if we hit a budget limit
                    return false;
                }
            }
            // If we counted all the elements and didn't bust, return true
            return true;
        }

        bool is_saturated(std::unordered_set<E *> &set)
        {
            // Make a copy of the budget to evaluate with
            std::vector<int> test_budget = budget;
            size_t part;
            for (auto el : set)
            {
                part = partition[el];
                test_budget[part] -= 1;
            }
            // If all test budget values are zero, constraint is saturated
            return std::all_of(test_budget.begin(), test_budget.end(), [](int rem_budget)
                               { return rem_budget == 0 });
        }
    };
}