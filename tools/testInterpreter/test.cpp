#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include "InterpretJson.h"
#include "game.h"
#include "list.h"

using namespace std;

int main(){
    Game g;
        
        string path = PATH_TO_JSON_TEST;
        
        InterpretJson j(path);
        
        cout << "Full json object: " << endl;
        cout << std::setw(4) << j.getData() << endl << endl;
        
        InterpretJson data(j);
        //Map json to game object
        data.interpret(g);
        
        //convert game object back to json (only works for config and not lists)
        json p = g;

        //Print config
        cout << " Data from game object:\n" << endl;
        cout << p << endl;


        //Manually print lists
        //Print setup
        cout << "\nsetup: " << endl;
        cout <<  "Rounds: " << g.setup()->getMapElement("Rounds")->getInt() << "\n";
        
        //Print constants
        cout << "\nconstants: " << endl;
        cout << "weapons: ";
        ElementVector _list =  g.constants()->getMapElement("weapons")->getVector();
        for (auto element : _list) {
            std::cout << "\nName: " << element->getMapElement("name")->getString() << "\t";
            std::cout << "Beats: " << element->getMapElement("beats")->getString() <<"\n";

        }

        //Print variables
        cout << "\nvariables: " << endl;
        cout << "winners: " << g.variables()->getMapElement("winners")->getString() << "\n";

        //Print per_player
        cout << "\nper_player: " << endl;
        cout << "wins: "  << g.per_player()->getMapElement("wins")->getInt() << "\n";
        
        //Print per_audience
        cout << "\nper_audience: "  << g.per_audience()->getString() << "\n";
        
        

    return 0;
}