#include <set>
#include <iostream>
#include <vector>

// WIP
// TODO: implement an updateable priority queue for arbitrary comparable classes (Elements)


class Element{
    public:
        int id;
        double value;

        // constructor with values
        Element(int id, double value){
            id = id;
            value = value;
        }

        // constructor without values
        Element(int id){
            id = id;
            value = 0;
        }

        Element(){}

        // overloading the < operator for elements
        bool operator< (const Element& el){
            if (el.value < value){
                return true;
            } else {
                return false;
            }
        }
};


class LazyGreedy{
    private:
        int curr_budget = 0;  // number of elements currently in set
        double curr_val = 0;  // current value of elements in set

    public:
        int n;  // holds size of ground set, indexed from 0 to n-1
        int budget;
        std::set <Element> curr_set;  // will hold elements selected to be in our set
        std::set <Element> marginals; // will hold marginals

        LazyGreedy(int N, int B){
            n = N;
            budget = B;
        };

        void run_greedy(double (*cost_function)(std::set <Element>)){
            int counter=0;
            first_iteration(cost_function);  // initializes marginals in first greedy iteration

            while (curr_budget < budget){
                counter++;
                lazy_greedy_step(cost_function);
                std::cout<< "Performed greedy algorithm iteration: " << counter << std::endl;
                print_status();
            }
        };

        void print_status(){
            std::cout << "Current set: {";
            for (auto el:curr_set){
                std::cout<< el.id << ",";
            }
            std::cout <<"}" << std::endl;
            std::cout<<"Current val: " << curr_val << std::endl;
        };

    private:
        void first_iteration(double (*F)(std::set <Element>)){
            /*std::set<Element> test_set;
            Element test_element;
            for (int i = 0; i < n; i++){
                test_element = Element(i, F(test_set)-curr_val);
                test_set.insert(test_element);
                marginals.emplace(test_element);
                test_set.erase(test_element);
            }
            test_element = ;
            */
        }

        void lazy_greedy_step(double (*F)(std::set <Element>)){
            std::set <Element> test_set(curr_set);
            double marginal_val = 0;
            double new_marginal_val;
            int new_element;

            // Can do lazy greedy with priority queue
            for(int i=0; i < n; i++){
                test_set.insert(i);  // add element to test set

                new_marginal_val = F(test_set) - curr_val;  // compute its value

                // if it is the best right now, remember it
                if (new_marginal_val > marginal_val){
                    marginal_val = new_marginal_val;
                    new_element = i;
                }

                // remove it from test set
                test_set = curr_set;  // Not the most efficient way to do this rn
            }

            // update the current set, value, and budget value with the found item
            curr_set.insert(new_element);
            curr_val = curr_val + marginal_val;
            curr_budget++;
        };
};