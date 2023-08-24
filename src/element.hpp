#pragma once
#include <cstddef>
#include <iostream>
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