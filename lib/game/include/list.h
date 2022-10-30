#pragma once

#include "server.h"

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
typedef std::map<Connection, std::shared_ptr<ListElement>> PlayerMap;

// INTERFACE
// The building block for all Lists
// Facilitates the construction of recursive multi-type lists
// Capable of holding ints, strings, vectors, & maps
class ListElement {
public:
    Type type;
    virtual ~ListElement(){}

    virtual ElementSptr clone() = 0;

    // Iterable //
    virtual bool getIterator(ElementVector::iterator& begin, ElementVector::iterator& end) = 0;
    virtual bool getIterator(ElementMap::iterator& begin, ElementMap::iterator& end) = 0;

    // Getters & Setters //
    virtual void setMapElement(std::string key, ElementSptr element) = 0;
    virtual ElementSptr getMapElement(std::string key) = 0;
    virtual void removeMapElement(std::string key) = 0;

    virtual ElementVector getSubList(std::string key) = 0;
    virtual std::string getString() = 0;

    virtual void addInt(int value) = 0;
    virtual void setInt(int value) = 0;
    virtual int getInt() = 0;

    virtual size_t getSize() = 0;

    // List Operations //
    virtual void extend(ElementSptr element) = 0;
    virtual void discard(unsigned count) = 0;

    virtual ElementSptr upfrom(int start) = 0;
};

template <typename T>
class Element : public ListElement {
    T _data;

public:
    Element(int data)              : _data(data) { type = Type::INT; }
    Element(std::string data)      : _data(data) { type = Type::STRING; }
    Element(ElementVector data)    : _data(data) { type = Type::VECTOR; }
    Element(ElementMap data)       : _data(data) { type = Type::MAP; }

    ElementSptr clone() final {
        if constexpr (std::is_same_v<T, ElementVector>) {
            ElementVector cloned;
            for (auto& element: _data) {
                cloned.push_back(element->clone());
            }
            return std::make_shared<Element<ElementVector>>(cloned); 
        } else if constexpr (std::is_same_v<T, ElementMap>) {
            ElementMap cloned;
            for (auto& [key, element]: _data) {
                cloned[key] = element->clone();
            }
            return std::make_shared<Element<ElementMap>>(cloned); 
        } else {
            return std::make_shared<Element<T>>(_data);
        }
    }


    bool getIterator(ElementVector::iterator& begin, ElementVector::iterator& end) final {
        if constexpr (std::is_same_v<T, ElementVector>) {
            begin = _data.begin();
            end = _data.end();
            return true;
        } else {
            return false;
        }
    }
        
    bool getIterator(ElementMap::iterator& begin, ElementMap::iterator& end) final {
        if constexpr (std::is_same_v<T, ElementMap>) {
            begin = _data.begin();
            end = _data.end();
            return true;
        } else {
            return false;
        }
    }


    void setMapElement(std::string key, ElementSptr element) final {
    // static_assert(std::is_same_v<T, ElementMap>, "setMapElement() must be called on a map");

        if constexpr (std::is_same_v<T, ElementMap>) {
            _data[key] = element;
        } else {
            // throw error //
        }
    }

    ElementSptr getMapElement(std::string key) final {
        // static_assert(std::is_same_v<T, ElementMap>, "getMapElement() must be called on a map");
        
        if constexpr (std::is_same_v<T, ElementMap>) {
            return _data[key];
        } else {
            // throw error //
            return nullptr;
        }
    }

    void removeMapElement(std::string key) final {
        // static_assert(std::is_same_v<T, ElementMap>, "removeMapElement() must be called on a map");
        
        if constexpr (std::is_same_v<T, ElementMap>) {
            _data.erase(key);
        } else {
            // throw error //
        }
    }

    ElementVector getSubList(std::string key) final {
        // static_assert(std::is_same_v<T, ElementVector>, "getSubList() must be called on a vector of maps");
        ElementVector sublist;

        if constexpr (std::is_same_v<T, ElementVector>) {
            for (auto element: _data) { 
                ElementSptr v = element->getMapElement(key);   
                if (v) {
                    sublist.push_back(v);
                } else {
                    // throw error //
                    // key not found in maps, return empty vector
                    return sublist;
                }
            }
        } else {
            // throw error //
        }
        return sublist;
    }

    std::string getString() final {
        // static_assert(std::is_same_v<T, std::string>, "getString() must be called on an string element");

        if constexpr (std::is_same_v<T, std::string>) {
            return _data;
        } else {
            // throw error //
            return "";
        }
    }

    void addInt(int value) final {
        // static_assert(std::is_integral_v<T>, "getInt() must be called on an int element");
        
        if constexpr (std::is_integral_v<T>) {
            _data += value;
        } else {
            // throw error //
        }
    }
    
    void setInt(int value) final {
        // static_assert(std::is_integral_v<T>, "getInt() must be called on an int element");
        
        if constexpr (std::is_integral_v<T>) {
            _data = value;
        } else {
            // throw error //
        }
    }

    int getInt() final {
        // static_assert(std::is_integral_v<T>, "getInt() must be called on an int element");
        
        if constexpr (std::is_integral_v<T>) {
            return _data;
        } else {
            // throw error //
            return 0;
        }
    }

    size_t getSize() final {
        // static_assert(std::is_integral_v<T>, "getInt() must be called on an int element");
        
        if constexpr (std::is_integral_v<T>) {
            // throw error //
            return 0;
        } else {
            return _data.size();
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
        // static_assert(std::is_same_v<T, ElementVector>, "discard() must be called on a vector");

        if constexpr (std::is_same_v<T, ElementVector>) {
            // discard <count> elements//
            for (unsigned i = 0; i < count; i++) _data.pop_back();
        } else {
            // throw error //
        }
    }

    // returns a list of the form { start, start+1, ... , data }
    // if start > data, returns an empty list
    // throws an error if the underlying data is not an int
    ElementSptr upfrom(int start) {
        // static_assert(std::is_integral_v<T>, "upfrom() must be called on an int");

        ElementVector list;
        if constexpr (std::is_integral_v<T>) {
            if (start <= _data) {
                for (int i = start; i <= _data; i++) {
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