#include "gtest/gtest.h"
#include <iostream>
#include <fstream>
#include "InterpretJson.h"
#include "game.h"
#include "list.h"
#include "rules.h"
#include "ASTVisitor.h"
#include "TreeBuilder.h"
#include <algorithm>


TEST(ASTTest, TestSplit){
    TreeBuilder treeBuilder;
    std::vector<std::string> expected = {"constants", ".", "weapon", "!=", "player", ".", "weapon"};
    auto splits = treeBuilder.split("constants.weapon != player.weapon");

    expected = {"(", "variables", ".", "winners",  "players", ".", "wins", ")"};
    auto splits2 = treeBuilder.split("(variables.winners->players.wins)");
    EXPECT_EQ(splits2, expected);
}

TEST(ASTTeest, TestBuilder){
    TreeBuilder treeBuilder;
    auto root = treeBuilder.buildTree("weapons->upfrom(1)");

    //EXPECT_EQ();
}
