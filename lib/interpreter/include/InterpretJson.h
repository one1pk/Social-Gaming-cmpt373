#pragma once

#include <nlohmann/json.hpp>
#include "game.h"

using namespace std;
using json = nlohmann::json;

using namespace ns;

class InterpretJson{
    public:

        InterpretJson(string path);
        InterpretJson(json j){
            data = j;
        }
        

        //maps json data to game object
        void interpret(Game& obj );

        json getData(){
            return data;
        }
    private:
        json data;
        
};