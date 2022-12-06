#pragma once

#include "server.h"

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <algorithm>


enum Type { // doesn't seem useful, might remove later 
    INT,
    STRING,
    VECTOR,
    BOOL,
    MAP,
    CONNECTION
};

class ListElement;

using ElementSptr = std::shared_ptr<ListElement>;
using ElementVector = std::vector<std::shared_ptr<ListElement>>;
using ElementMap = std::map<std::string, std::shared_ptr<ListElement>>;
using PlayerMap = std::map<User, std::shared_ptr<ListElement>>;

// INTERFACE
// The building block for all Lists
// Facilitates the construction of recursive multi-type lists
// Capable of holding ints, strings, vectors, & maps
class ListElement {
public:
    Type type;
    virtual ~ListElement(){}

    virtual ElementSptr clone() = 0;

    // Getters & Setters //
    virtual void setMapElement(std::string key, ElementSptr element) = 0;
    virtual ElementSptr getMapElement(std::string key) = 0;
    virtual void removeMapElement(std::string key) = 0;
    
    virtual ElementVector getSubList(std::string key) = 0;
    virtual ElementVector getVector() = 0;
    virtual ElementMap getMap() = 0;
    virtual std::string getString() = 0;
    virtual int getInt() = 0;
    virtual bool getBool() = 0;

    virtual void addInt(int value) = 0;
    virtual void setInt(int value) = 0;
    
    virtual size_t getSize() = 0;
    virtual int getSizeAsInt() = 0;

    virtual User getConnection() = 0;

    // List Operations //
    virtual void extend(ElementSptr element) = 0;
    virtual void discard(unsigned count) = 0;

    virtual ElementSptr upfrom(int start) = 0;
    virtual bool contains(ElementSptr element) = 0;
};

template <typename T>
class Element : public ListElement {
    T _data;

public:
    Element(int data)              : _data(data) { type = Type::INT; }
    Element(bool data)             : _data(data) { type = Type::BOOL; }
    Element(std::string data)      : _data(data) { type = Type::STRING; }
    Element(ElementVector data)    : _data(data) { type = Type::VECTOR; }
    Element(ElementMap data)       : _data(data) { type = Type::MAP; }
    Element(User data)       : _data(data) { type = Type::CONNECTION; }

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


    void setMapElement(std::string key, ElementSptr element) final {
        if constexpr (std::is_same_v<T, ElementMap>) {
            _data[key] = element;
        } else {
            // throw error //
        }
    }

    ElementSptr getMapElement(std::string key) final {      
        if constexpr (std::is_same_v<T, ElementMap>) {
            if (_data.find(key) != _data.end())
                return _data[key];
        } 
        return nullptr;
    }

    void removeMapElement(std::string key) final {        
        if constexpr (std::is_same_v<T, ElementMap>) {
            _data.erase(key);
        } else {
            // throw error //
        }
    }

    
    ElementVector getSubList(std::string key) final {
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

    
    ElementVector getVector() final {
        if constexpr (std::is_same_v<T, ElementVector>) {
            return _data;
        } else {
            // throw error //
            return {};
        }
    }

    ElementMap getMap() final {
        if constexpr (std::is_same_v<T, ElementMap>) {
            return _data;
        } else {
            return {};
        }
    }

    std::string getString() final {
        if constexpr (std::is_same_v<T, std::string>) {
            return _data;
        } else if constexpr (std::is_integral_v<T>) {
            return std::to_string(_data);
        } else if constexpr (std::is_same_v<T, bool>){
            std::string boolString;
            if(_data == true)
                boolString = "true";
            else
                boolString = "false";
            return boolString;
        } else {
            return "";
        }
    }

    void addInt(int value) final {        
        if constexpr (std::is_integral_v<T>) {
            _data += value;
        } else {
            // throw error //
        }
    }
    
    void setInt(int value) final {        
        if constexpr (std::is_integral_v<T>) {
            _data = value;
        } else {
            // throw error //
        }
    }

    int getInt() final {        
        if constexpr (std::is_integral_v<T>) {
            return _data;
        } else {
            // throw error //
            return 0;
        }
    }
    bool getBool() final {
        // static_assert(std::is_integral_v<T>, "getInt() must be called on an int element");
        
        if constexpr (std::is_same_v<T, bool>) {
            return _data;
        } 
        else if constexpr (std::is_integral_v<T>){
            if(_data == 1)
                return true;
            else if(_data == 0)
                return false;
        } 
        return false;
    }


    size_t getSize() final {        
        if constexpr (std::is_integral_v<T> || std::is_same_v<T, User> || std::is_same_v<T, bool>) {
            // throw error //
            return 0;
        } else {
            return _data.size();
        }
    }

    int getSizeAsInt() final {        
        if constexpr (std::is_integral_v<T> || std::is_same_v<T, User> || std::is_same_v<T, bool>) {
            // throw error //
            return 0;
        } else {
            return _data.size();
        }
    }

    User getConnection() final {
        if constexpr (std::is_same_v<T, User>) {
            return _data;
        } else {
            // throw error //
            return {0};
        }
    }


    void extend(ElementSptr elements) final {
        if constexpr (std::is_same_v<T, ElementVector>) {
            // extend //
            for (auto element: elements->getVector()) {
                _data.push_back(element);
            }
        } else {
            // throw error //
        }
    }

    void discard(unsigned count) final {
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
        ElementVector list;
        if constexpr (std::is_integral_v<T>) {
            for (int i = start; i <= _data; i++) {
                list.emplace_back(std::make_shared<Element<int>>(i));
            }
        }
        list.shrink_to_fit();
        return std::make_shared<Element<ElementVector>>(list);
    }

    bool contains(ElementSptr element){
        if constexpr (std::is_same_v<T, ElementVector>){
            return std::find_if(_data.begin(), _data.end(), [&element](auto& e){
                return e->getString() == element->getString();
            }) != _data.end();
        }
        else if constexpr (std::is_same_v<T, ElementMap>){
            return _data.find(element->getString()) != _data.end();
        }
        else{
            //throw error
            return false;
        }
    }
};


