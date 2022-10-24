#pragma once

#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;


class InterpretJson{
    public:

        InterpretJson(string path);

        //maps json data to game object
        template <class T>
        T& interpret(T& obj );


    private:
        json data;
        
};