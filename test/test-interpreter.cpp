#include "gtest/gtest.h"

#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include "InterpretJson.h"
#include "game.h"
#include "list.h"
#include "rules.h"

using Json = nlohmann::json;

string filePath = PATH_TO_JSON_TEST"/partRules.json";

class InterpreterTest : public ::testing::Test{
    protected:
        void SetUp() override{
            InterpretJson j(filePath);
            jsonData = j.getData();

            //Map json to game object
            j.interpret(g);
            
            //convert game object back to json
            gameDataToJson = g;

            setup = g.setup();
            constants = g.constants();
            variables = g.variables();
            per_player = g.per_player();
            per_audience = g.per_audience();
            rules = g.rules();
            rulesFromJson = g.rules_from_json();
        }

    Game g;
    InterpretJson j;

    ElementSptr setup;
    ElementSptr constants;
    ElementSptr variables;
    ElementSptr per_player;
    ElementSptr per_audience;
    RuleVector rules;

    ElementSptr rulesFromJson;
    //original data
    Json jsonData;

    //game data
    Json gameDataToJson;
};

TEST_F(InterpreterTest, MinimumConfigFromJSON) {
    EXPECT_EQ(g.name(), "Rock, Paper, Scissors");
    EXPECT_EQ(g.audience(), false);

    EXPECT_EQ(setup->getMapElement("Rounds")->getInt(), 4);

    ElementVector _list =  constants->getMapElement("weapons")->getVector();
    EXPECT_EQ(_list[0]->getMapElement("name")->getString(), "Rock");
    EXPECT_EQ(_list[0]->getMapElement("beats")->getString(), "Scissors");
    EXPECT_EQ(_list[1]->getMapElement("name")->getString(), "Paper");
    EXPECT_EQ(_list[1]->getMapElement("beats")->getString(), "Rock");
    EXPECT_EQ(_list[2]->getMapElement("name")->getString(), "Scissors");
    EXPECT_EQ(_list[2]->getMapElement("beats")->getString(), "Paper");

    EXPECT_EQ(variables->getMapElement("winners")->getString(), "{}");

    EXPECT_EQ(per_player->getMapElement("wins")->getInt(), 0);
    EXPECT_EQ(per_player->getMapElement("weapon")->getString(), "");
    EXPECT_EQ(per_audience->getString(), "{}");
    
   
}

//std::cout << " Data from game object:\n" << std::endl;
//std::cout << setw(4) << gameDataToJson << std::endl;

TEST_F(InterpreterTest, NamesAreResolved){
    ElementSptr listToTest = j.resolveName(g, "constants");
    
    //std::cout << listToTest->getMapElement("weapons")->getVector()[2]->getMapElement("beats")->getString() << std::endl;
    EXPECT_EQ(listToTest, constants);

    size_t weaponsSize = listToTest->getMapElement("weapons")->getSize();
    EXPECT_EQ(weaponsSize, 3);
}

TEST_F(InterpreterTest, RulesGetCorrectLists){
    ElementSptr expectedList = setup->getMapElement("Rounds")->upfrom(1);
    ElementSptr forEachList = dynamic_cast<Foreach&>(*rules[0]).getList();

    int round3 =  forEachList->getVector()[2]->getInt();
    int sizeForEachList = forEachList->getSize();

    EXPECT_EQ(round3, 3);
    EXPECT_EQ(sizeForEachList, 4);
}

TEST_F(InterpreterTest, MinimumConfigToJSON) {
    EXPECT_EQ(jsonData, gameDataToJson);
}

