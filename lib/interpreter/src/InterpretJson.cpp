#include <fstream>
#include <iostream>
#include "InterpretJson.h"

using namespace std;
using Json = nlohmann::json;

InterpretJson::InterpretJson(string path){
    try{
    ifstream f(path);
    Json jData = Json::parse(f);
    f.close();
    data = jData;
    } catch (std::exception& e){
        //TODO: Integrate glog instead of using cout
        cout << "error reading file" << e.what() << endl;
    }
}

InterpretJson::InterpretJson(Json j){
    data = j;
}

Json InterpretJson::getData(){
    return data;
}

void InterpretJson::interpret(Game& game){
    game = data.get<Game>();
}

void InterpretJson::interpretWithRules(Game& game){
    game = data.get<Game>();

    //This stores all the content of the rules as strings
    ElementSptr rulesFromJson;
    data.at("rules").get_to(rulesFromJson);

    //convert ElementSptr to rule vector containing rule objects
    RuleVector rules;

    ElementMap gameListsMap = {{"constants", game.constants()},
        {"variables", game.variables()},
        {"setup", game.setup()},
        {"per-player", game.per_player()},
        {"per-audience", game.per_audience()}};

    TreeBuilder treeBuilder(gameListsMap);

    toRuleVec(game, rulesFromJson, rules);
    game.setRules(rules);

    game.setID();
    game.setName("TestGame");
    game.setStatusCreated();
}


//Interpret Rules
void InterpretJson::toRuleVec(Game& game, const ElementSptr& rules_from_json, RuleVector& rule_vec) {

    for (auto rule: rules_from_json->getVector()){
        std::string ruleName = rule->getMapElement("rule")->getString();
        RuleSptr ruleObject;

        if(ruleName == "foreach"){
            std::string listString = rule->getMapElement("list")->getString();
            auto listExpressionRoot = treeBuilder.buildTree(listString);

            RuleVector subRules;
            toRuleVec(game, rule->getMapElement("rules"), subRules);
            ruleObject = std::make_shared<Foreach>(listExpressionRoot, subRules);
        }

        else if(ruleName == "global-message"){
            auto msg = rule->getMapElement("value")->getString();
            ruleObject = std::make_shared<GlobalMsg>(msg, game._global_msgs);
        }

        else if(ruleName == "parallelfor"){
            RuleVector subRules;
            toRuleVec(game, rule->getMapElement("rules"), subRules);
            ruleObject = std::make_shared<ParallelFor>(game._players, subRules);
        }

        else if(ruleName == "input-choice"){
            std::string choicesString = rule->getMapElement("choices")->getString();
            std::shared_ptr<ASTNode> choicesExpressionRoot = treeBuilder.buildTree(choicesString);

            auto prompt = rule->getMapElement("prompt")->getString();
            auto result = rule->getMapElement("result")->getString();
            auto timeout = rule->getMapElement("timeout")->getInt();
            ruleObject = std::make_shared<InputChoice>(prompt, choicesExpressionRoot, timeout, result, game._player_msgs, game._player_input);
        }

        else if (ruleName == "add"){
            auto to = rule->getMapElement("to")->getString();
            auto value = rule->getMapElement("value");
            ruleObject = std::make_shared<Add>(to, value);
        }

        else if (ruleName == "scores"){
            auto score = rule->getMapElement("score")->getString();
            auto ascending = rule->getMapElement("ascending")->getBool();
            ruleObject = std::make_shared<Scores>(game._players, score, ascending, game._global_msgs);
        }

        else if (ruleName == "extend"){
            std::string targetString = rule->getMapElement("target")->getString();
            std::shared_ptr<ASTNode> targetExpressionRoot = treeBuilder.buildTree(targetString);

            std::string listString = rule->getMapElement("list")->getString();
            std::shared_ptr<ASTNode> listExpressionRoot = treeBuilder.buildTree(listString);
            ruleObject = std::make_shared<Extend>(targetExpressionRoot, listExpressionRoot);
        }

        else if (ruleName == "discard"){
            std::string fromString = rule->getMapElement("from")->getString();
            std::shared_ptr<ASTNode> fromExpressionRoot = treeBuilder.buildTree(fromString);

            std::string countString = rule->getMapElement("count")->getString();
            std::shared_ptr<ASTNode> countExpressionRoot = treeBuilder.buildTree(countString);
            ruleObject = std::make_shared<Extend>(fromExpressionRoot, countExpressionRoot);
        }

        rule_vec.push_back(ruleObject);
    }
}



