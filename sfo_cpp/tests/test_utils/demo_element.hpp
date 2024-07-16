// A sample element consisting of a unique int ID and a double value.
#pragma once
#include <iostream>
#include <unordered_set>

class Element
{
public:
    int id;
    double value;
    // constructor with values
    Element(int id, double value)
    {
        this->id = id;
        this->value = value;
    }
    // constructor without values
    Element(int id)
    {
        this->id = id;
        this->value = 666;
    }
    Element(const Element &el)
    {
        this->id = el.id;
        this->value = el.value;
    }
    Element()
    {
        this->id = 999;
        this->value = 111;
    }
    // overloading the < operator for elements
    bool operator<(const Element &el)
    {
        if (this->value >= el.value)
        {
            return true;
        }
        else
        {
            return false;
        };
    }

    bool operator==(const Element &el)
    {
        if (this->id == el.id)
        {
            return true;
        }
        else
        {
            return false;
        };
    }

    friend bool operator==(const Element &e1, const Element &e2);
    friend bool operator!=(const Element &e1, const Element &e2);
    friend bool operator<(const Element &e1, const Element &e2);
    friend std::ostream &operator<<(std::ostream &os, const Element &el);
    void set_value(const double &val)
    {
        this->value = val;
    }
};

namespace std
{
    template <>
    struct hash<Element>
    {
        auto operator()(const Element &el) const -> size_t
        {
            size_t id_hash = std::hash<int>()(el.id);
            return id_hash; // ^ val_hash;
        }
    };
}

std::ostream &operator<<(std::ostream &os, const Element &el)
{
    os << el.id << "~" << el.value;
    return os;
}

std::ostream &operator<<(std::ostream &os, const std::unordered_set<Element *> &set)
{
    os << "{";
    for (auto el : set)
    {
        os << *el << ",";
    }
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const std::unordered_set<Element> &set)
{
    os << "{";
    for (auto el : set)
    {
        os << el << ",";
    }
    os << "}";
    return os;
}

bool operator==(const Element &e1, const Element &e2)
{
    return e1.id == e2.id;
}

bool operator!=(const Element &e1, const Element &e2)
{
    return e1.id != e2.id;
}

bool operator<(const Element &e1, const Element &e2)
{
    return e1.value <= e2.value;
}

std::unordered_set<Element *> *generate_ground_set(int &set_size)
{
    // helper function to generate a ground set of set_size elements
    std::unordered_set<Element *> *V = new std::unordered_set<Element *>;
    Element *el;
    int id = 0;
    for (int i = 0; i < set_size; i++)
    {
        id++;
        el = new Element;
        el->id = id;
        el->value = id * id;
        V->insert(el);
    }
    return V;
}