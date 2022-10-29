#include "gtest/gtest.h"

#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include "InterpretJson.h"
#include "game.h"


TEST(Interpreter, MinimumGameConfig) {
    Game g;
        
    string path = PATH_TO_JSON_TEST;
    
    InterpretJson j(path);
    
    cout << "Full json object: " << endl;
    cout << j.getData() << endl << endl;
    
    InterpretJson data(j);
    data.interpret(g);
    cout << "Data from game object: name, audience, player count min and max: " << endl;
    // g.printInfo();

    EXPECT_EQ(g.getConfigInfo(), "name: Rock, Paper, Scissors, audience: false, min. players: 2, max. players: 4");
}
