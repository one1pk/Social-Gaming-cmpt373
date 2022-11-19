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


TEST(ASTTest, TestSplit){
    ExpressionTree expressionTree;
    std::deque<std::string> expected = {"constants", ".", "weapon", "!=", "player", ".", "weapon"};
    auto splits = expressionTree.split("constants.weapon != player.weapon");

    expected = {"(", "variables", ".", "winners",  "players", ".", "wins", ")"};
    auto splits2 = expressionTree.split("(variables.winners->players.wins)");
    EXPECT_EQ(splits2, expected);
}

TEST(ASTTest, ResolverTest){
    Game game;
    std::string path = PATH_TO_JSON_TEST"/rock_paper_scissors.json";
    InterpretJson j(path);
    j.interpretWithRules(game);
    
    
    ExpressionResolver resolver;

    ElementMap gameListsMap = {{"constants", game.constants()},
        {"variables", game.variables()},
        {"setup", game.setup()},
        {"per-player", game.per_player()},
        {"per-audience", game.per_audience()}};

    std::shared_ptr<ASTNode> root;
    ExpressionTree expressionTree(gameListsMap, root);
    std::string expression = "setup.Rounds.upfrom(1)";
    expressionTree.build(expression);
    auto expressionRoot = expressionTree.getRoot();
    ElementMap elementsMap;
    expressionRoot->accept(resolver, elementsMap);

    //auto expected = game.setup()->getMapElement("Round")->upfrom(1)->getVector();
    EXPECT_EQ(4, game.setup()->getMapElement("Rounds")->upfrom(1)->getVector().size());
    //EXPECT_EQ(4, resolver.getResult()->getVector().size());
    
}

