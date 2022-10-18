#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include "InterpretJson.h"
#include "game.h"

using namespace std;
using json = nlohmann::json;


Interpret(string path){
    this->path = path;
    ifstream f(path);
    json jData = json::parse(f);
    f.close();
    this->data = jData;
}


Game interpret(Game& game){}

}
