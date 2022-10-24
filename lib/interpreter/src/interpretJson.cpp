#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include "InterpretJson.h"

using namespace std;
using json = nlohmann::json;


    InterpretJson::InterpretJson(string path){
        ifstream f(path);
        json jData = json::parse(f);
        data = jData;
    }

    template<class T>
    T& interpret(T& obj){}


