#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

enum Type {
    INT,
    STRING,
    VECTOR,
    MAP,
};

class ListElement;

typedef std::shared_ptr<ListElement> ElementSptr; // shared pointer to a list element
typedef std::vector<std::shared_ptr<ListElement>> ElementVector; // a vector of shared pointers to list elements
typedef std::map<std::string, std::shared_ptr<ListElement>> ElementMap; // a map of of shared pointers to list elements (with string keys)

// INTERFACE
// The building block for all Lists
// Facilitates the construction of recursive multi-type lists
// Capable of holding ints, strings, vectors, & maps
class ListElement {
public:
    Type type;
    virtual ~ListElement(){}

    // Iterable //
    virtual bool getIterator(ElementVector::iterator& begin, ElementVector::iterator& end) = 0;
    virtual bool getIterator(ElementMap::iterator& begin, ElementMap::iterator& end) = 0;

    // Getters //
    virtual ElementSptr getMapElement(std::string key) = 0;
    virtual int getInt() = 0;

    // List Operations //
    virtual void extend(ElementSptr element) = 0;
    virtual void discard(unsigned count) = 0;

    virtual ElementSptr upfrom(int start) = 0;
};

template <typename T>
class Element : public ListElement {
    T _value;

public:
    Element(int value)              : _value(value) { type = Type::INT; }
    Element(std::string value)      : _value(value) { type = Type::STRING; }
    Element(ElementVector value)    : _value(value) { type = Type::VECTOR; }
    Element(ElementMap value)       : _value(value) { type = Type::MAP; }

    bool getIterator(ElementVector::iterator& begin, ElementVector::iterator& end) final {
        if constexpr (std::is_same_v<T, ElementVector>) {
            begin = _value.begin();
            end = _value.end();
            return true;
        } else {
            return false;
        }
    }
        
    bool getIterator(ElementMap::iterator& begin, ElementMap::iterator& end) final {
        if constexpr (std::is_same_v<T, ElementMap>) {
            begin = _value.begin();
            end = _value.end();
            return true;
        } else {
            return false;
        }
    }


    ElementSptr getMapElement(std::string key) final {
        // static_assert(std::is_same_v<T, ElementMap>, "getMapElement() must be called on a map");
        
        // TODO: check if key exists in map
        if constexpr (std::is_same_v<T, ElementMap>) {
            return _value[key];
        } else {
            // throw error //
            return nullptr;
        }
    }

    int getInt() final {
        // static_assert(std::is_integral_v<T>, "getInt() must be called on an int element");
        
        if constexpr (std::is_integral_v<T>) {
            return _value;
        } else {
            // throw error //
            return 0;
        }
    }


    void extend(ElementSptr element) final {
        // static_assert(std::is_same_v<T, ElementVector> || std::is_same_v<T, ElementMap>,
        //     "extend() must be called on an map or a vector"
        // );

        if constexpr (std::is_same_v<T, ElementMap>) {
            if (element->type == Type::MAP) {
                // extend //
                // element must be a map
            } else {
                // throw error //
            } 
        } else if constexpr (std::is_same_v<T, ElementVector>) {
            if (element->type == Type::MAP) {
                // throw error //
            } else {
                // extend //
                // element could be another vector
                // or it could be a primitive (string/int)
            }
        } else {
            // throw error //
            // cannot extend strings or ints
        }
    }

    void discard(unsigned count) final {
        // static_assert(std::is_same_v<T, ElementVector> || std::is_same_v<T, ElementMap>,
        //     "discard() must be called on an map or a vector"
        // );

        if constexpr (std::is_same_v<T, ElementVector> || std::is_same_v<T, ElementMap>) {
            // discard <count> elements//
        } else {
            // throw error //
        }
    }

    // returns a list of the form { start, start+1, ... , value }
    // if start > value, returns an empty list
    // throws an error if the underlying value is not an int
    ElementSptr upfrom(int start) {
        // static_assert(std::is_integral_v<T>, "upfrom() must be called on an int");

        ElementVector list;
        if constexpr (std::is_integral_v<T>) {
            if (start <= _value) {
                for (int i = start; i <= _value; i++) {
                    list.emplace_back(std::make_shared<Element<int>>(i));
                }
            }
        }
        return std::make_shared<Element<ElementVector>>(list);
    }
};



////////////////////////////////////////////////////////////////////////
// alternate implementation, keeping in case we want to switch methods

// class Element {
// public:
//     Type type;
//     virtual ~Element() = 0; // pure virtual function makes the class abstract (cannot be instantiated)
//     virtual void extend(std::shared_ptr<Element> element) {}
//     virtual void discard(unsigned count) {}
//     virtual std::shared_ptr<Element> upfrom(int start) { return nullptr; }
// };
// Element::~Element() {} // must define the destructor implementation

// typedef std::shared_ptr<Element> ElementSptr; // shared pointer to a list element (could be a map, vector, int, string)
// typedef std::vector<std::shared_ptr<Element>> ElementVector; // a vector of shared pointers to list elements
// typedef std::map<std::string, std::shared_ptr<Element>> ElementMap; // a map of of shared pointers to list elements (with string keys)

// class Map : public Element {
//     ElementMap _value;
// public:
//     Map(ElementMap value) : _value(value) { type = Type::MAP; }
    
//     void extend(ElementSptr element) final {
//         if (element->type == Type::MAP) {
//             // extend //
//             // element must be a map
//         } else {
//             // throw error //
//         }
//     }

//     void discard(unsigned count) final {
//         // discard <count> elements from map
//     }
// };

// class Vector : public Element {
//     ElementVector _value;
// public:
//     Vector(ElementVector value) : _value(value) { type = Type::VECTOR; }

//     void extend(ElementSptr element) final {
//         if (element->type == Type::MAP) {
//             // throw error //
//         } else {
//             // extend //
//             // element could be another vector
//             // or it could be a primitive (string/int)
//         }
//     }

//     void discard(unsigned count) final {
//         // discard the last <count> elements from vector
//     }
// };

// class Int : public Element {
//     int _value;
// public:
//     Int(int value) : _value(value) { type = Type::INT; }

//     ElementSptr upfrom(int start) {
//         ElementVector list;
//         for (int i = start; i <= _value; i++) {
//             list.emplace_back(std::make_shared<Int>(i));
//         }
//         return std::make_shared<Vector>(list);
//     }
// };

// class String : public Element {
//     std::string _value;
// public:
//     String(std::string value) : _value(value) { type = Type::STRING; }
// };




////////////////////////////////////////////////////////////////////////
// old implementation, keeping in case we want to reuse something

// class List {
// public:
//     List(std::map<std::string, std::unique_ptr<Element>> list);
//     void extend(std::unique_ptr<Element> element);
//     void extend(List* other_list);
//     // void reverse();
//     // void shuffle();
//     // void sort();
//     // void sort(std::string attribute);
//     // void deal(List &to, unsigned count);
//     void discard(unsigned count);

//     // size_t size();
//     // std::vector<std::any> sublist(std::string attribute);
//     // std::map<std::string, std::any> element(std::string attribute, std::any value);
//     // bool contains(std::string attribute, std::any value);
//     // // collect ??

// private:
//     std::map<std::string, std::unique_ptr<Element>> _list;
//     // std::vector<std::string> elements;
// };