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
        InterpretJson() = default;
        InterpretJson(string path);
        InterpretJson(Json j);
        
        void interpret(Game& obj );

        //TEMP: just fo testing purposes
        void interpretWithRules(Game& obj );

        Json getData();
    
        Json data;
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
    j.at("configuration").at("setup").get_to(g._setup);
    j.at("constants").get_to(g._constants);
    j.at("variables").get_to(g._variables);
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
    j["configuration"]["setup"] = g._setup;
    j["variables"] = g._variables;
    j["constants"] = g._constants;
    j["per-player"] = g._per_player;
    j["per-audience"] = g._per_audience;
}










/*static std::unordered_map<std::string,TypeR> const table = { {"foreach",TypeR::FOREACH}, {"global-message",TypeR::GLOBALMSG} };
TypeR getType(std::string str){
    auto it = table.find(str);
    if (it != table.end()) {
    return it->second;
    } else { return DEFAULT; }
}
inline void from_json(const json& j, shared_ptr<Foreach>& r){
    r = make_shared<Foreach>();
    j.at("list").get_to(r->list);
    j.at("rules").get_to(r->rules);
}

inline void from_json(const json& j, shared_ptr<GlobalMsg>& r){
    r = make_shared<GlobalMsg>();
    r->msg = j.at("value").get<std::string>();
}


inline void from_json(const json& j, RuleSptr& r){
    
   
        switch (getType(j.at("rule"))){
            case FOREACH:{
                r = j.get<shared_ptr<Foreach>>();
                break;
            }
            case GLOBALMSG:{
                r = j.get<shared_ptr<GlobalMsg>>();
                break;
            }
            default:
                break;
        }
    
}
inline void from_json(const json& j, RuleVector& r){
    if(j.is_array()){
        for(auto it : r){
        r = std::make_shared<RuleSptr>(j.get<RuleSptr>());
        }
    }
}
*/


