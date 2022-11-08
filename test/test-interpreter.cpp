#include "gtest/gtest.h"

#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include "InterpretJson.h"
#include "game.h"
#include "list.h"

using json = nlohmann::json;


TEST(Interpreter, MinimumConfigFromJSON) {
    Game g;
        
    string path = PATH_TO_JSON_TEST;
    
    InterpretJson j(path);
    //Map json to game object
    j.interpret(g);

    EXPECT_EQ(g.name(), "Rock, Paper, Scissors");
    EXPECT_EQ(g.audience(), false);

    ElementSptr setup = g.setup();
    ElementSptr constants = g.constants();
    ElementSptr variables = g.variables();
    ElementSptr per_player = g.per_player();
    ElementSptr per_audience = g.per_audience();
    
    
    EXPECT_EQ(setup->getMapElement("Rounds")->getInt(), 10);

    ElementVector _list =  constants->getMapElement("weapons")->getVector();
    EXPECT_EQ(_list[0]->getMapElement("name")->getString(), "Rock");
    EXPECT_EQ(_list[0]->getMapElement("beats")->getString(), "Scissors");
    EXPECT_EQ(_list[1]->getMapElement("name")->getString(), "Paper");
    EXPECT_EQ(_list[1]->getMapElement("beats")->getString(), "Rock");
    EXPECT_EQ(_list[2]->getMapElement("name")->getString(), "Scissors");
    EXPECT_EQ(_list[2]->getMapElement("beats")->getString(), "Paper");

    EXPECT_EQ(variables->getMapElement("winners")->getString(), "");

    EXPECT_EQ(per_player->getMapElement("wins")->getInt(), 0);
    EXPECT_EQ(per_player->getMapElement("weapon")->getString(), "");
    EXPECT_EQ(per_audience->getString(), "");

}

TEST(Interpreter, MinimumConfigToJSON) {
    Game g;
        
    string path = PATH_TO_JSON_TEST;
    
    InterpretJson j(path);
    j.interpret(g);
    //convert game object back to json (only works for config and not lists)
    //json p = g;
}