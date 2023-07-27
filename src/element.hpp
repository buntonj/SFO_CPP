#pragma once

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