#include <iostream>
#include <fstream>
#include "InterpretJson.h"
#include "game.h"
#include <glog/logging.h>

using namespace std;
using json = nlohmann::json;


InterpretJson::InterpretJson(string path) {
    // google::InitGoogleLogging("Interpreter");
    try {
        ifstream f(path);
        json jData = json::parse(f);
        f.close();
        data = jData;
    } catch (std::exception& e) {
        std::cout << "error reading file" << e.what() << endl;
    }
}


void InterpretJson::interpret(Game& obj) {
    obj = data.get<Game>();
}


