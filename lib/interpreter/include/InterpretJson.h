#pragma once

#include "game.h"
#include <nlohmann/json.hpp>
#include "rules.h"
#include "ASTVisitor.h"
#include "ExpressionTree.h"

using namespace std;
using Json = nlohmann::json;


class InterpretJson{
    public:
        InterpretJson(string game_name, User owner);
        InterpretJson(std::string path);
        Game interpret();
    
        Json data;
        std::string game_name;
        User owner;
        ExpressionTree expressionTree;
        void toRuleVec(Game& game, const ElementSptr& rules_from_json, RuleVector& rule_vec);
};

//recursively maps Json data to list element
inline void from_json(const Json& j, ElementSptr& e){
    if(j.is_string()) e = std::make_shared<Element<std::string>>(j.get<std::string>());
    else if(j.is_number()) e = std::make_shared<Element<int>>(j.get<int>());
    else if(j.is_boolean()) e = std::make_shared<Element<bool>>(j.get<bool>());
    else if(j.is_array()) e = std::make_shared<Element<ElementVector>>(j.get<ElementVector>());
    else if(j.is_object()) e = std::make_shared<Element<ElementMap>>(j.get<ElementMap>());
}

inline void from_json(const Json& j,  Game& g){
    j.at("configuration").at("name").get_to(g._name);
    j.at("configuration").at("audience").get_to(g._has_audience);
    j.at("configuration").at("player count").at("min").get_to(g._player_count.min);
    j.at("configuration").at("player count").at("max").get_to(g._player_count.max);
    j.at("configuration").at("setup").get_to(g._game_state["setup"]);
    j.at("constants").get_to(g._game_state["constants"]);
    j.at("variables").get_to(g._game_state["variables"]);
    j.at("per-player").get_to(g._per_player);
    j.at("per-audience").get_to(g._per_audience);
}

inline void to_json(Json& j, const ElementSptr& e){
     switch(e->type) {
 		case Type::STRING:
 			j = e->getString();
 			break;
 		case Type::INT:
 			j = e->getInt();
 			break;
         case Type::BOOL:
 			j = e->getBool();
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

 inline void to_json( Json& j, const Game& g){
    j["configuration"]["name"] = g._name;
    j["configuration"]["audience"] = g._has_audience;
    j["configuration"]["player count"]["min"] = g._player_count.min;
    j["configuration"]["player count"]["max"] = g._player_count.max;
    //j["configuration"]["setup"] = g._game_state["setup"];
    //j["variables"] = g._game_state["variables"];
    //j["constants"] = g._game_state["constants"];
    j["per-player"] = g._per_player;
    j["per-audience"] = g._per_audience;
 }