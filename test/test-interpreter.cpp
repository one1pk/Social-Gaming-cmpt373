#include "gtest/gtest.h"

#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include "InterpretJson.h"
#include "game.h"
#include "list.h"
#include "rules.h"
#include <algorithm>

using Json = nlohmann::json;

string filePath = PATH_TO_JSON_TEST"/partRules.json";

//derived to include toRuleVec so it does not interfere with gameserver
class InterpretJsonTest : public InterpretJson{
    public:
    InterpretJsonTest(std::string filePath) : InterpretJson(filePath){}
    InterpretJsonTest() : InterpretJson() {}
    InterpretJsonTest(Json j) : InterpretJson(j) {}

    void interpretTest(Game& game){
        game = data.get<Game>();
        //convert ElementSptr _rules_from_json to rule vector containing rule objects _rules
        toRuleVec(game, game.rules_from_json(), game.rules());
    }
};

class InterpreterTest : public ::testing::Test{
    protected:
        void SetUp() override{
            InterpretJsonTest j(filePath);
            jsonData = j.getData();

            //Map json to game object
            j.interpretTest(g);
            
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
    InterpretJsonTest j;

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

TEST_F(InterpreterTest, Split){
    std::vector<std::string> splits = j.splitString("setup.Rounds.upfrom(1)");
    std::vector<std::string> splitsExpected = {"setup", "Rounds", "upfrom", "1"};

    EXPECT_EQ(splits, splitsExpected);

    std::vector<std::string> splits2 = j.splitString("constants.weapons");
    std::vector<std::string> splitsExpected2 = {"constants", "weapons"};

    EXPECT_EQ(splits2, splitsExpected2);
}

TEST_F(InterpreterTest, NamesAreResolved){
    ElementSptr listToTest1 = j.resolveName(g, "variables");
    EXPECT_EQ(listToTest1, variables);

    ElementSptr listToTest2 = j.resolveName(g, "constants");
    EXPECT_EQ(listToTest2, constants);

    ElementSptr listToTest3 = j.resolveName(g, "constants.weapons");
    EXPECT_EQ(listToTest3, constants->getMapElement("weapons"));    
}

TEST_F(InterpreterTest, OperationsTest){
    //test sublist
    ElementSptr listToTest4 = j.resolveName(g, "constants.weapons.sublist(name)");
    std::vector<std::string> expectedList = {"Rock", "Paper", "Scissors"};
    std::vector<std::string> stringVector;
    for(auto it : listToTest4->getVector()){
        stringVector.push_back(it->getString());
    }
    EXPECT_EQ(stringVector, expectedList);

    //test size
    ElementSptr sizeOfList = j.resolveName(g, "constants.weapons.size");
    EXPECT_EQ(sizeOfList->getInt(), 3);
}


TEST_F(InterpreterTest, RulesGetCorrectLists){
    ElementSptr forEachList = dynamic_cast<Foreach&>(*rules[0]).getList();
    std::vector<int> numVector;
    std::vector<int> expectedNumVector = {1,2,3,4};
    for(auto it : forEachList->getVector()){
        numVector.push_back(it->getInt());
    }
    
    EXPECT_EQ(numVector, expectedNumVector);
}

TEST_F(InterpreterTest, MinimumConfigToJSON) {
    EXPECT_EQ(jsonData, gameDataToJson);
}

TEST_F(InterpreterTest, RunGame){
    g.run();
}