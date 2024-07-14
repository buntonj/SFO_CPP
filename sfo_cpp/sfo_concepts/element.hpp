#pragma once
#include <cstddef>
#include <iostream>
#include <queue>
#include <unordered_set>

template <typename E>
class compare_element_value_pair
{
public:
    bool operator()(const std::pair<E *, double> &lhs, const std::pair<E *, double> &rhs)
    {
        return lhs.second < rhs.second;
    };
};

template <typename E>
using LazyGreedyQueue = std::priority_queue<std::pair<E *, double>, std::vector<std::pair<E *, double>>, compare_element_value_pair<E>>;