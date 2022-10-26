#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include "InterpretJson.h"
#include "game.h"

using namespace ns;
using namespace std;
int main(){
    Game g;
        //string path = "/CMPT373/gamingplatform/social-gaming/tools/gameserver/test.json";
        //InterpretJson j(path);
        auto j = R"(
        {
            "configuration": {
                "name": "Rock, Paper, Scissors",
                "player_count": {"min": 2, "max": 4},
                "audience": false,
                "setup": {
                    "Rounds": 10
                }
            }
        }
        )"_json;
        cout << "Full json object: " << endl;
        cout << j << endl << endl;
        
        InterpretJson data(j);
        data.interpret(g);
        cout << "Data from game object: name, audience, player count min and max: " << endl;
        g.printInfo();

    return 0;
}