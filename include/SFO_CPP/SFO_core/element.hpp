#pragma once
#include <cstddef>
#include <iostream>
#include <queue>
#include <unordered_set>

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
            if (this-> value >= el.value){
                return true;
            } else{
                return false;
            };
        }

        bool operator==(const Element& el){
            if (this->id == el.id){
                return true;
            } else{
                return false;
            };
        }

        friend bool operator== (const Element& e1, const Element& e2);
        friend bool operator!= (const Element& e1, const Element& e2);
        friend bool operator< (const Element& e1, const Element& e2);
        friend std::ostream& operator<<(std::ostream& os, const Element& el);
        void set_value(const double& val){
            value = val;
        }
};

namespace std {
template <>
struct hash<Element> {
    auto operator()(const Element &el) const -> size_t {
        size_t id_hash = std::hash<int>()(el.id);
        // size_t val_hash = std::hash<double>()(el.value);
        return id_hash; // ^ val_hash;
    }
    };
}

std::ostream& operator<<(std::ostream& os, const Element& el){
    os << el.id << "~" << el.value;
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::unordered_set<Element*> &set){
    os << "{";
    for (auto el:set){
        os << *el << ",";
    }
    os << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::unordered_set<Element> &set){
    os << "{";
    for (auto el:set){
        os << el << ",";
    }
    os << "}";
    return os;
}

bool operator== (const Element& e1, const Element& e2){
    return e1.id == e2.id;
}

bool operator!= (const Element& e1, const Element& e2){
    return e1.id != e2.id;
}

bool operator< (const Element& e1, const Element& e2){
    return e1.value <= e2.value;
}

class compare_elements{
    public:
        bool operator()(const std::pair<Element*, double> &lhs, const std::pair<Element*, double> &rhs){
            return lhs.second < rhs.second;
        };
};

typedef std::priority_queue<std::pair<Element*, double>,std::vector<std::pair<Element*,double>>,compare_elements> LazyGreedyQueue;

std::unordered_set<Element*>* generate_ground_set(int &N){
    // helper function to generate a ground set of N elements
    std::unordered_set<Element*> *V = new std::unordered_set<Element*>;
    Element* el;
    int id = 0;
    for (int i=0; i<N; i++){
        id++;
        el = new Element;
        el->id = id;
        el->value = 0;
        V->insert(el);
    }
    return V;
}