#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include "Interpret.h"

using namespace std;
using json = nlohmann::json;

class InterpretJson{
    public:
    Interpret(string path){
        ifstream f(path);
        json jData = json::parse(f);
        this.data = jData;
    }

    T& interpret(T& obj);

}