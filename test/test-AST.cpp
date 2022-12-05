#include "gtest/gtest.h"
#include <iostream>
#include <fstream>
#include "InterpretJson.h"
#include "game.h"
#include "list.h"
#include "rules.h"
#include "ASTVisitor.h"
#include "ExpressionTree.h"
#include "ExpressionResolver.h"
#include <algorithm>
#include "TreePrinter.h"

class ASTTest : public ::testing::Test{
protected:
    void SetUp() override{
        std::string path = PATH_TO_JSON_TEST"/rock_paper_scissors.json";
        InterpretJson j(path);
        game = j.interpret();
        Connection c1;
        Connection c2;
        Connection c3;
        c1.id = 1;
        c2.id = 2;
        c3.id = 3;
        game.addPlayer(c1);
        game.addPlayer(c2);
        game.addPlayer(c3);
    }

public:
    Game game;
    std::string expression;
    ExpressionResolver resolver;
    void resolve();
};

void ASTTest::resolve(){
    ElementMap gameListsMap = {{"constants", game.constants()},
        {"variables", game.variables()},
        {"setup", game.setup()},
        {"per-player", game.per_player()},
        {"per-audience", game.per_audience()}};

    
    ExpressionTree expressionTree(gameListsMap, game._players);
    expressionTree.build(expression);
    auto expressionRoot = expressionTree.getRoot();
    
    TreePrinter treePrinter;
    ElementMap elementsMap;
    expressionRoot->accept(resolver, elementsMap);

    std::cout << std::endl;
    expressionRoot->accept(treePrinter, elementsMap);
    std::cout << std::endl;
}

TEST_F(ASTTest, TestSplit){
    ExpressionTree expressionTree;
    std::deque<std::string> expected = {"constants",  "weapon", "!=", "player", ".", "weapon"};
    auto splits = expressionTree.splitString("constants->weapon != player.weapon");

}

TEST_F(ASTTest, TestOperationResolution){
    expression = "variables.winners.size";
    resolve();
    EXPECT_EQ(0, resolver.getResult()->getVector().size());

    expression = "setup.Rounds.upfrom(1)";
    auto expected = game.setup()->getMapElement("Rounds")->upfrom(1)->getVector();
    resolve();
    EXPECT_EQ(4, resolver.getResult()->getVector().size());
    
    expression = "constants.weapons.name";
    auto choices = game.constants()->getMapElement("weapons")->getSubList("name");
    resolve();
    EXPECT_EQ(resolver.getResult()->getVector(), choices);
    EXPECT_EQ(resolver.getResult()->getVector()[1]->getString(), "Paper");


    expression = "variables.winners.size == 0";
    resolve();
    EXPECT_EQ(resolver.getResult()->getBool(), true);
}

TEST_F(ASTTest, TestPlayersListResolution){
    std::string str = "rock";
    auto element = std::make_shared<Element<std::string>>(str);
    auto players = game._players;
    for(auto playerMap : *players){
        playerMap.second->setMapElement("weapon", element);
    }
    
    expression = "players.weapon";
    resolve();
    
    ElementVector weapons;
    for(auto it : *players){
        weapons.emplace_back(it.second->getMapElement("weapon"));
    }
    
    EXPECT_EQ(resolver.getResult()->getVector(), weapons);
    EXPECT_EQ(resolver.getResult()->getSizeAsInt(), 3);

    expression = "!players.weapon.contains(rock)";
    resolve();
    EXPECT_EQ(resolver.getResult()->getBool(), false);

    expression = "players.name";
    resolve();
    for(auto it : resolver.getResult()->getVector())
        std::cout << it->getString() << std::endl;
}


