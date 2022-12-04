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


TEST(ASTTest, TestSplit){
    ExpressionTree expressionTree;
    std::deque<std::string> expected = {"constants",  "weapon", "!=", "player", ".", "weapon"};
    auto splits = expressionTree.split("constants->weapon != player.weapon");

}

static void resolve(Game& game, ExpressionResolver& resolver, std::string expression){
    ElementMap gameListsMap = {{"constants", game.constants()},
        {"variables", game.variables()},
        {"setup", game.setup()},
        {"per-player", game.per_player()},
        {"per-audience", game.per_audience()}};

    Connection c1;
    Connection c2;
    Connection c3;
    c1.id = 1;
    c2.id = 2;
    c3.id = 3;
    game.addPlayer(c1);
    game.addPlayer(c2);
    game.addPlayer(c3);

    std::shared_ptr<ASTNode> root;
    ExpressionTree expressionTree(root, gameListsMap, game._players);
    expressionTree.build(expression);
    auto expressionRoot = expressionTree.getRoot();
    
    TreePrinter treePrinter;
    ElementMap elementsMap;
    
    expressionRoot->accept(resolver, elementsMap);

    std::cout << std::endl;
    expressionRoot->accept(treePrinter, elementsMap);
    std::cout << std::endl;
}

TEST(ASTTest, ResolverTest){
    Game game;
    std::string path = PATH_TO_JSON_TEST"/rock_paper_scissors.json";
    InterpretJson j(path);
    j.interpretWithRules(game);

    Json g = game;

    //std::cout << setw(4) << g << std::endl;
    
    std::string expression = "variables.winners.size";
    ExpressionResolver resolver;
    resolve( game, resolver, expression);
    EXPECT_EQ(0, resolver.getResult()->getVector().size());


    expression = "setup.Rounds.upfrom(1)";
    auto expected = game.setup()->getMapElement("Rounds")->upfrom(1)->getVector();
    resolve(game, resolver, expression);
    EXPECT_EQ(4, resolver.getResult()->getVector().size());
    
    expression = "constants.weapons.name";
    auto choices = game.constants()->getMapElement("weapons")->getSubList("name");
    resolve(game, resolver, expression);
    EXPECT_EQ(resolver.getResult()->getVector(), choices);
    EXPECT_EQ(resolver.getResult()->getVector()[1]->getString(), "Paper");



    //testing players list resolving
    std::string str = "rock";
    auto element = std::make_shared<Element<std::string>>(str);
    auto players = game._players;
    for(auto playerMap : *players){
        playerMap.second->setMapElement("weapon", element);
    }
    
    expression = "players.weapon";
    resolve(game, resolver, expression);
    
    
    ElementVector weapons;
    for(auto it : *players){
        weapons.emplace_back(it.second->getMapElement("weapon"));
    }
    
    EXPECT_EQ(resolver.getResult()->getVector(), weapons);
    EXPECT_EQ(resolver.getResult()->getSizeAsInt(), 3);

    expression = "!players.weapon.contains(rock)";
    resolve(game, resolver, expression);
    EXPECT_EQ(resolver.getResult()->getBool(), false);

   
    expression = "variables.winners.size == 0";
    resolve(game, resolver, expression);
    EXPECT_EQ(resolver.getResult()->getBool(), true);

    expression = "players.name";
    resolve(game, resolver, expression);
    for(auto it : resolver.getResult()->getVector())
        std::cout << it->getString() << std::endl;
}


