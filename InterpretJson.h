#pragma once

#include <nlohmann/json.hpp>
#include "game.h"

class InterpretJson{
    public:

        Interpret(string path);

        //maps json data to game object
        virtual Game interpret(Game& game );

    private:
        string path;
        json data;
}