#pragma once
#include <queue>
#include <unordered_set>
#include <iostream>
#include <vector>
#include <cfloat>
#include "element.hpp"
#include "cost_function.hpp"
#include "constraint.hpp"


class LazyGreedy{
    private:
        double curr_val = 0;  // current value of elements in set
        constraint::Constraint *constraint;
        bool constraint_saturated = false;
        int MAXITER = 50;

    public:
        int n;  // holds size of ground set, indexed from 0 to n-1
        std::unordered_set <Element> curr_set;  // will hold elements selected to be in our set
        std::priority_queue <Element> marginals; // will hold marginals

        LazyGreedy(const int &N){
            n = N;
        }

        LazyGreedy(const int &N, const int &B){
            n = N;
            add_constraint(new constraint::Knapsack(B));
        };

        void run_greedy(costfunction::CostFunction &cost_function){
            clear_marginals();
            int counter=1;
            first_iteration(cost_function);  // initializes marginals in first greedy iteration
            std::cout<< "Performed lazy greedy algorithm iteration: " << counter << std::endl;
            print_status();
            while (!constraint_saturated && counter < MAXITER){
                counter++;
                lazy_greedy_step(cost_function);
                std::cout<< "Performed lazy greedy algorithm iteration: " << counter << std::endl;
                print_status();
            }
        };

        void run_greedy(costfunction::CostFunction &cost_function, bool cost_benefit){
            clear_marginals();
            if(!cost_benefit){
                run_greedy(cost_function);
            } else if(constraint::Knapsack* k = dynamic_cast<constraint::Knapsack*>(constraint); k != nullptr){
                int counter = 1;
                double budget = 0;
                cost_benefit_first_iteration(cost_function, k, budget);  // initializes marginals in first greedy iteration
                std::cout<< "Performed lazy greedy algorithm iteration: " << counter << std::endl;
                print_status();
                while (!constraint_saturated && counter < MAXITER){
                    counter++;
                    cost_benefit_lazy_greedy_step(cost_function, k, budget);
                    std::cout<< "Performed lazy greedy algorithm iteration: " << counter << std::endl;
                    print_status();
            }
            }
        };

        void print_status(){
            std::cout << "Current set:" << curr_set << std::endl;
            std::cout<<"Current val: " << curr_val << std::endl;
        };

        void add_constraint(constraint::Constraint *C){
            constraint = C;
        }

    private:

        // Special function for first iteration, populates priority queue
        void first_iteration(costfunction::CostFunction &F){
            std::unordered_set<Element> test_set;
            Element candidate;

            // iterate through all candidate elements
            for (int i = 0; i < n; i++){
                candidate.id = i;
                test_set.insert(candidate);

                // First check if element is feasible
                if(constraint->test_membership(test_set)){
                    std::cout<<"test set:" << test_set << std::endl;
                    std::cout<< "F(set) : " << F(test_set) << std::endl;
                    candidate.set_value(F(test_set) - curr_val);
                } else {
                    candidate.set_value(-DBL_MAX);
                }

                // note that <Element> is sorted by VALUE
                marginals.push(candidate);  // insert element into priority queue with priority
                std::cout<<"MARGINALS top:" << marginals.top().id << "~" << marginals.top().value;
                std::cout<<" size:" << marginals.size() << std::endl;
                test_set = curr_set; // refresh test set
            }

            if (marginals.top().value < 0){
                constraint_saturated = true;  // no element gives benefit and/or no feasible element
            } else{
                curr_set.insert(marginals.top());  // pull top element
                curr_val = curr_val + marginals.top().value;
                marginals.pop();  // pop it
                constraint_saturated = constraint->is_saturated(curr_set);
            }
        }

                // Special function for first iteration, populates priority queue
        void cost_benefit_first_iteration(costfunction::CostFunction &F, constraint::Knapsack* K, double curr_budget){
            std::unordered_set<Element> test_set;
            Element candidate;

            // iterate through all candidate elements
            for (int i = 0; i < n; i++){
                candidate.id = i;
                test_set.insert(candidate);

                // First check if element is feasible
                if(constraint->test_membership(test_set)){
                    std::cout<<"test set:" << test_set << std::endl;
                    std::cout<< "F(set) : " << F(test_set) << std::endl;
                    candidate.set_value((F(test_set) - curr_val)/(K->value(candidate)));
                } else {
                    candidate.set_value(-DBL_MAX);
                }

                // note that <Element> is sorted by VALUE
                marginals.push(candidate);  // insert element into priority queue with priority
                std::cout<<"MARGINALS top:" << marginals.top().id << "~" << marginals.top().value;
                std::cout<<" size:" << marginals.size() << std::endl;
                test_set = curr_set; // refresh test set
            }

            if (marginals.top().value < 0){
                constraint_saturated = true;  // no element gives benefit and/or no feasible element
            } else{
                curr_set.insert(marginals.top());  // pull top element
                curr_val = curr_val + marginals.top().value;
                curr_budget = curr_budget + K->value(curr_set);
                marginals.pop();  // pop it
                constraint_saturated = constraint->is_saturated(curr_set);
            }
        }

        void lazy_greedy_step(costfunction::CostFunction &F){
            std::unordered_set <Element> test_set = curr_set;
            Element candidate;

            // iterate through all candidate element IDs
            for (int i=0; i < n; i++){
                candidate = marginals.top();
                test_set.insert(candidate);  // pull top of current queue
                marginals.pop();  // pop this element from queue
                
                // First check if element is feasible
                if(constraint->test_membership(test_set)){
                    std::cout<<"test set:" << test_set << std::endl;
                    std::cout<< "F(set) : " << F(test_set) << std::endl;
                    candidate.set_value(F(test_set) - curr_val);
                } else {
                    candidate.set_value(-DBL_MAX);
                }

                candidate.set_value(F(test_set) - curr_val);  // reset its priority value
                marginals.push(candidate);  // add it back
                std::cout<<"MARGINALS top:" << marginals.top().id << "~" << marginals.top().value;
                std::cout<<" size:" << marginals.size() << std::endl;
                if (marginals.top() == candidate){ // if it didn't change position, then it is the best option, break
                    break;
                }
                test_set = curr_set;       
            }

            if(marginals.top().value < 0){
                constraint_saturated = true;  // nothing of value could be added OR constraint full
            } else{
                // update the current set, value, and budget value with the found item
                curr_set.insert(marginals.top());
                curr_val = curr_val + marginals.top().value;
                marginals.pop();
                constraint_saturated = constraint->is_saturated(curr_set); // allows for early stop detection
            }
        };

        void cost_benefit_lazy_greedy_step(costfunction::CostFunction &F, constraint::Knapsack* K, double curr_budget){
            std::unordered_set <Element> test_set = curr_set;
            Element candidate;

            // iterate through all candidate element IDs
            for (int i=0; i < n; i++){
                candidate = marginals.top();
                test_set.insert(candidate);  // pull top of current queue
                marginals.pop();  // pop this element from queue
                
                // First check if element is feasible
                if(constraint->test_membership(test_set)){
                    std::cout<<"test set:" << test_set << std::endl;
                    std::cout<< "F(set) : " << F(test_set) << std::endl;
                    candidate.set_value((F(test_set) - curr_val)/(K->value(candidate)));
                } else {
                    candidate.set_value(-DBL_MAX);
                }

                candidate.set_value(F(test_set) - curr_val);  // reset its priority value
                marginals.push(candidate);  // add it back
                std::cout<<"MARGINALS top:" << marginals.top().id << "~" << marginals.top().value;
                std::cout<<" size:" << marginals.size() << std::endl;
                if (marginals.top() == candidate){ // if it didn't change position, then it is the best option, break
                    break;
                }
                test_set = curr_set;       
            }

            if(marginals.top().value < 0){
                constraint_saturated = true;  // nothing of value could be added OR constraint full
            } else{
                // update the current set, value, and budget value with the found item
                curr_set.insert(marginals.top());
                curr_val = curr_val + marginals.top().value;
                marginals.pop();
                constraint_saturated = constraint->is_saturated(curr_set); // allows for early stop detection
            }
        };

        void clear_marginals(){
            std::priority_queue<Element> empty;
            std::swap(marginals, empty);
        }
};