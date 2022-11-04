#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include "InterpretJson.h"
#include "game.h"

using namespace std;
using json = nlohmann::json;


InterpretJson::InterpretJson(string path){
    ifstream f(path);
    json jData = json::parse(f);
    f.close();
    data = jData;
}


void InterpretJson::interpret(Game& obj){
    obj = data.get<Game>();
}


