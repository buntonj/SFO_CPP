#include <queue>
#include <set>
#include <iostream>
#include <vector>

// WIP
// TODO: have cost_function overloaded to act on single elements, not just sets of elements
// TODO: have cost function become member object?


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
            value = 666;
        }

        Element(const Element& el){
            id = el.id;
            value = el.value;
        }

        Element(){
            id = 999;
            value = 111;
        }

        // overloading the < operator for elements
        bool operator< (const Element& el){
            if (el.value >= value){
                return true;
            } else {
                return false;
            }
        }

        friend bool operator== (const Element& e1, const Element& e2);
        friend bool operator!= (const Element& e1, const Element& e2);
        friend bool operator< (const Element& e1, const Element& e2);

        void set_value(const double& val){
            value = val;
        }
};

bool operator== (const Element& e1, const Element& e2){
    return e1.id == e2.id;
}

bool operator!= (const Element& e1, const Element& e2){
    return e1.id != e2.id;
}

bool operator< (const Element& e1, const Element& e2){
    return e1.value <= e2.value;
}


class LazyGreedy{
    private:
        int curr_budget = 0;  // number of elements currently in set
        double curr_val = 0;  // current value of elements in set

    public:
        int n;  // holds size of ground set, indexed from 0 to n-1
        int budget;
        std::set <Element> curr_set = std::set<Element>();  // will hold elements selected to be in our set
        std::priority_queue <Element> marginals; // will hold marginals

        LazyGreedy(int N, int B){
            n = N;
            budget = B;
        };

        void run_greedy(double (*cost_function)(std::set <Element>)){
            int counter=1;
            first_iteration(cost_function);  // initializes marginals in first greedy iteration
            std::cout<< "Performed greedy algorithm iteration: " << counter << std::endl;
            print_status();
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

        // Special function for first iteration, populates priority queue
        void first_iteration(double (*F)(std::set <Element>)){
            std::set<Element> test_set;
            Element test_el;

            // iterate through all candidate elements
            for (int i = 0; i < n; i++){
                test_el.id = i;
                test_set.insert(test_el);
                // test_set.emplace(5, 111);  // add element to test set
                
                std::cout<<"test set: {";
                for(auto a:test_set){
                    std::cout<< a.id << "~" << a.value << ", ";
                }
                std::cout<<"}";
                // std::cout<< test_set.size() << std::endl;
                std::cout<< "F(set) : " << F(test_set) << std::endl;
                test_el.set_value(F(test_set) - curr_val);
                marginals.push(test_el);  // insert element into priority queue with priority
                std::cout<<"MARGINALS top:" << marginals.top().id << "~" << marginals.top().value;
                std::cout<<" size:" << marginals.size() << std::endl;
                test_set = curr_set; // refresh test set
            }
            curr_set.insert(marginals.top());  // pull top element
            curr_val = curr_val + marginals.top().value;
            marginals.pop();  // pop it
            curr_budget++;  // update budget
        }

        void lazy_greedy_step(double (*F)(std::set <Element>)){
            std::set <Element> test_set = curr_set;
            Element el;

            // iterate through all candidate element IDs
            for (int i=0; i < n; i++){
                el.id = marginals.top().id;
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