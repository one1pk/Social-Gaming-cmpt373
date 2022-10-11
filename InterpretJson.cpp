#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include "Interpret.cpp"

using namespace std;
using json = nlohmann::json;

class InterpretJson : public Interpret{
    public:
    Interpret(string path){
        ifstream f(path);
        json jData = json::parse(f);
        this.data = jData;
    }

    @override
    T& interpret(T& obj);

}