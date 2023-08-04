#pragma once
#include <queue>
#include <unordered_set>
#include <iostream>
#include <vector>
#include "element.hpp"
#include "cost_function.hpp"


class LazyGreedy{
    private:
        int curr_budget = 0;  // number of elements currently in set
        double curr_val = 0;  // current value of elements in set

    public:
        int n;  // holds size of ground set, indexed from 0 to n-1
        int budget;
        std::unordered_set <Element> curr_set;  // will hold elements selected to be in our set
        std::priority_queue <Element> marginals; // will hold marginals

        LazyGreedy(int N, int B){
            n = N;
            budget = B;
        };

        void run_greedy(CostFunction &cost_function){
            int counter=1;
            first_iteration(cost_function);  // initializes marginals in first greedy iteration
            std::cout<< "Performed lazy greedy algorithm iteration: " << counter << std::endl;
            print_status();
            while (curr_budget < budget){
                counter++;
                lazy_greedy_step(cost_function);
                std::cout<< "Performed lazy greedy algorithm iteration: " << counter << std::endl;
                print_status();
            }
        };

        void print_status(){
            std::cout << "Current set:" << curr_set << std::endl;
            std::cout<<"Current val: " << curr_val << std::endl;
        };

    private:

        // Special function for first iteration, populates priority queue
        void first_iteration(CostFunction &F){
            std::unordered_set<Element> test_set;
            Element candidate;

            // iterate through all candidate elements
            for (int i = 0; i < n; i++){
                candidate.id = i;
                test_set.insert(candidate);
                // test_set.emplace(5, 111);  // add element to test set
                
                std::cout<<"test set:" << test_set << std::endl;
                std::cout<< "F(set) : " << F(test_set) << std::endl;
                candidate.set_value(F(test_set) - curr_val);

                // note that <Element> is sorted by VALUE
                marginals.push(candidate);  // insert element into priority queue with priority
                std::cout<<"MARGINALS top:" << marginals.top().id << "~" << marginals.top().value;
                std::cout<<" size:" << marginals.size() << std::endl;
                test_set = curr_set; // refresh test set
            }
            curr_set.insert(marginals.top());  // pull top element
            curr_val = curr_val + marginals.top().value;
            marginals.pop();  // pop it
            curr_budget++;  // update budget
        }

        void lazy_greedy_step(CostFunction &F){
            std::unordered_set <Element> test_set = curr_set;
            Element el;

            // iterate through all candidate element IDs
            for (int i=0; i < n; i++){
                el = marginals.top();
                test_set.insert(el);  // pull top of current queue
                marginals.pop();  // pop this element from queue
                // test_set.insert(el);  // add it to testing set
                std::cout<<"test set: {";
                for(auto a:test_set){
                    std::cout<< a.id << "~" << a.value << ", ";
                }
                std::cout<<"}";
                std::cout<< "F(set) : " << F(test_set) << std::endl;

                el.set_value(F(test_set) - curr_val);  // reset its priority value
                marginals.push(el);  // add it back
                std::cout<<"MARGINALS top:" << marginals.top().id << "~" << marginals.top().value;
                std::cout<<" size:" << marginals.size() << std::endl;
                if (marginals.top() == el){ // if it didn't change position, then it is the best option, break
                    break;
                }
                test_set = curr_set;       
            }

            // update the current set, value, and budget value with the found item
            curr_set.insert(marginals.top());
            curr_val = curr_val + marginals.top().value;
            marginals.pop();
            curr_budget++;
        };
};