#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <nlohmann/json.hpp>
#include "InterpretJson.h"
#include "rules.h"
#include "list.h"
#include "game.h"

using json = nlohmann::json;

std::string filePath = PATH_TO_JSON_TEST"/trivia_no_rules.json";

class NewRulesTest : public ::testing::Test{
    protected:
        void SetUp() override{
            InterpretJson j(filePath);

            //Map json to game object
            j.interpret(g);

            setup = g.setup();
            constants = g.constants();
            variables = g.variables();
            per_player = g.per_player();
            per_audience = g.per_audience();
        }
    Game g;
    InterpretJson j;
    ElementSptr setup, constants, variables, per_player, per_audience;
};

TEST_F(NewRulesTest, DealToAttribute){
    ElementSptr sfrom_list =  std::make_shared<Element<ElementVector>>(constants->getMapElement("qa-bank")->getSubList("question"));

    std::make_unique<Deal>(sfrom_list, variables->getMapElement("current-question"), 1)->execute(constants->getMapElement("qa-bank"));

    EXPECT_EQ(variables->getMapElement("current-question")->getString(), "What game is this?");
}

TEST_F(NewRulesTest, SortWithKey){
    EXPECT_EQ(g.name(), "Trivia!");

    ElementVector test_list = constants->getMapElement("qa-bank")->getVector();
    EXPECT_EQ(test_list[3]->getMapElement("question")->getString(), "What game is this?");
    EXPECT_EQ(test_list[3]->getMapElement("answer")->getString(), "Trivia!");

    ElementSptr stest_list =  std::make_shared<Element<ElementVector>>(test_list);
    std::make_unique<Sort>(stest_list, "question")->execute(stest_list);

    EXPECT_EQ(stest_list->getVector()[0]->getMapElement("question")->getString(), "In what city is SFU's main campus?");
    EXPECT_EQ(stest_list->getVector()[3]->getMapElement("question")->getString(), "What is the capital of Canada?");
}

TEST(RulesTest, SortWithoutKey_Int){
    ElementVector test_list;
    test_list.push_back(make_shared<Element<int>>(5));
    test_list.push_back(make_shared<Element<int>>(1));
    test_list.push_back(make_shared<Element<int>>(3));
    test_list.push_back(make_shared<Element<int>>(4));
    ElementSptr stest_list =  std::make_shared<Element<ElementVector>>(test_list);

    std::make_unique<Sort>(stest_list)->execute(stest_list);
    EXPECT_EQ(stest_list->getVector()[0]->getInt(), 1);
    EXPECT_EQ(stest_list->getVector()[1]->getInt(), 3);
    EXPECT_EQ(stest_list->getVector()[2]->getInt(), 4);
    EXPECT_EQ(stest_list->getVector()[3]->getInt(), 5);
}

