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

static ExpressionResolver& build(Game& game, ExpressionResolver resolver, std::string expression){
    ElementMap gameListsMap = {{"constants", game.constants()},
        {"variables", game.variables()},
        {"setup", game.setup()},
        {"per-player", game.per_player()},
        {"per-audience", game.per_audience()}};

    std::shared_ptr<ASTNode> root;
    ExpressionTree expressionTree(gameListsMap, root);
    expressionTree.build(expression);
    auto expressionRoot = expressionTree.getRoot();
    
    TreePrinter treePrinter;
    ElementMap elementsMap;

    expressionRoot->accept(resolver, elementsMap);

    std::cout << std::endl;
    expressionRoot->accept(treePrinter, elementsMap);
    std::cout << std::endl;

    return resolver;
}

TEST(ASTTest, ResolverTest){
    Game game;
    std::string path = PATH_TO_JSON_TEST"/rock_paper_scissors.json";
    InterpretJson j(path);
    j.interpret(game);

    
    std::string expression = "variables.winners.size";
    ExpressionResolver resolver;
    resolver = build( game, resolver, expression);
    EXPECT_EQ(0, resolver.getResult()->getVector().size());


    expression = "setup.Rounds.upfrom(1)";
    auto expected = game.setup()->getMapElement("Rounds")->upfrom(1)->getVector();
    resolver = build(game, resolver, expression);
    EXPECT_EQ(4, expected.size());
    
    expression = "constants.weapons.sublist(name)";
    auto choices = game.constants()->getMapElement("weapons")->getSubList("name");
    resolver = build(game, resolver, expression);
    EXPECT_EQ(resolver.getResult()->getVector(), choices);
}

