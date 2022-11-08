#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "InterpretJson.h"
#include "game.h"
#include "list.h"

using namespace std;
using json = nlohmann::json;

int main(){
    Game g;
        
    string path = PATH_TO_JSON_TEST;
    
    InterpretJson j(path);
    
    cout << "Full json object: " << endl;
    cout << std::setw(4) << j.getData() << endl << endl;
    
    //InterpretJson data(j);
    //Map json to game object
    j.interpret(g);
    ElementSptr setup = g.setup();
    ElementSptr constants = g.constants();
    ElementSptr variables = g.variables();
    ElementSptr per_player = g.per_player();
    ElementSptr per_audience = g.per_audience();
    //convert game object back to json (only works for config and not lists)
    json p = g;

    //Print config
    cout << " Data from game object:\n" << endl;
    cout << std::setw(4) <<  p << endl;


    //Manually print lists
    //Print setup
    cout << "\nsetup: " << endl;
    cout <<  "Rounds: " << setup->getMapElement("Rounds")->getInt() << "\n";
    
    
    //Print constants
    cout << "\nconstants: " << endl;
    cout << "weapons: ";
    ElementVector _list =  constants->getMapElement("weapons")->getVector();
    for (auto element : _list) {
        std::cout << "\nName: " << element->getMapElement("name")->getString() << "\t";
        std::cout << "Beats: " << element->getMapElement("beats")->getString() <<"\n";

    }

    //Print variables
    cout << "\nvariables: " << endl;
    cout << "winners: " << variables->getMapElement("winners")->getString() << "\n";

    //Print per_player
    cout << "\nper_player: " << endl;
    cout << "wins: "  << per_player->getMapElement("wins")->getInt() << "\n";
    cout << "weapon: "  << per_player->getMapElement("weapon")->getString() << "\n";
    
    
    //Print per_audience
    cout << "\nper_audience: "  << per_audience->getString() << "\n";
    
        

    return 0;
}