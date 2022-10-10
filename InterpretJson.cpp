#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include "Interpret.h"

using namespace std;
using json = nlohmann::json;

class InterpretJson{
    Interpret(string path){
        ifstream f(path);
        json data = json::parse(f);
        this.data = data;
    }

    T interpret(T obj);
}