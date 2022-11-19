#include "gtest/gtest.h"
#include <iostream>
#include <fstream>
#include "InterpretJson.h"
#include "game.h"
#include "list.h"
#include "rules.h"
#include "ASTVisitor.h"
#include "ExpressionTree.h"
#include <algorithm>


TEST(ASTTest, TestSplit){
    ExpressionTree expressionTree;
    std::vector<std::string> expected = {"constants", ".", "weapon", "!=", "player", ".", "weapon"};
    auto splits = expressionTree.split("constants.weapon != player.weapon");

    expected = {"(", "variables", ".", "winners",  "players", ".", "wins", ")"};
    auto splits2 = expressionTree.split("(variables.winners->players.wins)");
    EXPECT_EQ(splits2, expected);
}


