#pragma once

#include <nlohmann/json.hpp>
#include "game.h"

using namespace std;
using json = nlohmann::json;

class InterpretJson{
    public:
        InterpretJson() = default;
        InterpretJson(string path);
       

        InterpretJson(json j){
            data = j;
        }
        

        //maps json data to game object
        void interpret(Game& obj );

        json getData(){
            return data;
        }
    private:
        json data;
        
};

//recursively maps json data to list element
inline void from_json(const json&j, ElementSptr &e){
        if(j.is_string()) e = std::make_shared<Element<std::string>>(j.get<std::string>());
        else if(j.is_number()) e = std::make_shared<Element<int>>(j.get<int>());
        else if(j.is_array()) e = std::make_shared<Element<ElementVector>>(j.get<ElementVector>());
	    else if(j.is_object()) e = std::make_shared<Element<ElementMap>>(j.get<ElementMap>());
    }

inline void to_json(json&j, const ElementSptr &e){
    switch(e->type) {
		case Type::STRING:
			j = e->getString();
			break;
		case Type::INT:
			j = e->getInt();
			break;
        case Type::MAP:{
            j = e->getMap();;
            break;
        }
        case Type::VECTOR:{
            j = e->getVector();
            break;
        }
        default:
            break;
	}
}

inline void from_json(const json &j,  Game &g){
    j.at("configuration").at("name").get_to(g._name);
    j.at("configuration").at("audience").get_to(g._has_audience);
    j.at("configuration").at("player count").at("min").get_to(g._player_count.min);
    j.at("configuration").at("player count").at("max").get_to(g._player_count.max);
    j.at("configuration").at("setup").get_to(g._setup);
    j.at("constants").get_to(g._constants);
    j.at("variables").get_to(g._variables);
    j.at("per-player").get_to(g._per_player);
    j.at("per-audience").get_to(g._per_audience);
}

//only works for config, not urgent
inline void to_json( json &j, const Game &g){
    j["configuration"]["name"] = g._name;
    j["configuration"]["audience"] = g._has_audience;
    j["configuration"]["player count"]["min"] = g._player_count.min;
    j["configuration"]["player count"]["max"] = g._player_count.max;
    j["configuration"]["setup"] = g._setup;
    j["variables"] = g._variables;
    j["constants"] = g._constants;
    j["per-player"] = g._per_player;
    j["per-audience"] = g._per_audience;
}
