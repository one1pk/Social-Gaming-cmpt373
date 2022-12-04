#include <fstream>
#include <iostream>
#include "InterpretJson.h"

using namespace std;
using Json = nlohmann::json;

InterpretJson::InterpretJson(string path){
    try{
    ifstream f(path);
    Json jData = nlohmann::ordered_json::parse(f);
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

    std::shared_ptr<ASTNode> root;
    expressionTree = ExpressionTree(root, gameListsMap, game._players);

    toRuleVec(game, rulesFromJson, rules);
    game.setRules(rules);

    game.setID();
    game.setName("TestGame");
    game.setStatusCreated();
}


//Interpret Rules
void InterpretJson::toRuleVec(Game& game, const ElementSptr& rules_from_json, RuleVector& rule_vec) {

    for (auto& rule: rules_from_json->getVector()){
        std::string ruleName = rule->getMapElement("rule")->getString();
        RuleSptr ruleObject;

        if(ruleName == "foreach"){
            std::string listString = rule->getMapElement("list")->getString();
            expressionTree.build(listString);
            auto listExpressionRoot = expressionTree.getRoot();

            RuleVector subRules;
            toRuleVec(game, rule->getMapElement("rules"), subRules);
            auto elementName = rule->getMapElement("element")->getString();
            ruleObject = std::make_shared<Foreach>(listExpressionRoot, subRules, elementName);
        }

        else if(ruleName == "global-message"){
            auto msg = rule->getMapElement("value")->getString();
            ruleObject = std::make_shared<GlobalMsg>(msg, game._global_msgs);
        }

        else if(ruleName == "parallelfor"){
            RuleVector subRules;
            toRuleVec(game, rule->getMapElement("rules"), subRules);
            auto elementName = rule->getMapElement("element")->getString();
            ruleObject = std::make_shared<ParallelFor>(game._players, subRules, elementName);
        }

        else if(ruleName == "when"){
            std::shared_ptr<When> when = std::make_shared<When>();
            
            ElementVector cases = rule->getMapElement("cases")->getVector();
            for(auto& caseRulePair : cases){
                std::cout << caseRulePair->getMapElement("condition")->getString() << std::endl;
                std::string conditionString = caseRulePair->getMapElement("condition")->getString();
                expressionTree.build(conditionString);
                std::shared_ptr<ASTNode> conditionExpressionRoot = expressionTree.getRoot();

                RuleVector caseRules;
                toRuleVec(game, caseRulePair->getMapElement("rules"), caseRules);
                when->condition_rule_pairs.push_back({conditionExpressionRoot, caseRules});
            }
            when->set();
            ruleObject = when;
        }

        else if(ruleName == "input-choice"){
            std::string choicesString = rule->getMapElement("choices")->getString();
            expressionTree.build(choicesString);
            std::shared_ptr<ASTNode> choicesExpressionRoot = expressionTree.getRoot();

            auto prompt = rule->getMapElement("prompt")->getString();
            auto result = rule->getMapElement("result")->getString();
            auto timeout = rule->getMapElement("timeout")->getInt();
            ruleObject = std::make_shared<InputChoice>(prompt, choicesExpressionRoot, timeout, result, game._player_msgs, game._player_input);
        }

        else if (ruleName == "add"){
            auto toString = rule->getMapElement("to")->getString();
            expressionTree.build(toString);
            std::shared_ptr<ASTNode> toExpressionRoot = expressionTree.getRoot();

            auto value = rule->getMapElement("value");
            ruleObject = std::make_shared<Add>(toExpressionRoot, value);
        }

        else if (ruleName == "scores"){
            auto score = rule->getMapElement("score")->getString();
            auto ascending = rule->getMapElement("ascending")->getBool();
            ruleObject = std::make_shared<Scores>(game._players, score, ascending, game._global_msgs);
        }

        else if (ruleName == "discard"){
            std::string fromString = rule->getMapElement("from")->getString();
            expressionTree.build(fromString);
            std::shared_ptr<ASTNode> fromExpressionRoot = expressionTree.getRoot();

            std::string countString = rule->getMapElement("count")->getString();
            expressionTree.build(countString);
            std::shared_ptr<ASTNode> countExpressionRoot = expressionTree.getRoot();
            ruleObject = std::make_shared<Discard>(fromExpressionRoot, countExpressionRoot);
        }

        else if (ruleName == "extend"){
            std::string targetString = rule->getMapElement("target")->getString();
            expressionTree.build(targetString);
            std::shared_ptr<ASTNode> targetExpressionRoot = expressionTree.getRoot();

            std::string listString = rule->getMapElement("list")->getString();
            expressionTree.build(listString);
            std::shared_ptr<ASTNode> listExpressionRoot = expressionTree.getRoot();
            ruleObject = std::make_shared<Extend>(targetExpressionRoot, listExpressionRoot);
        }

        rule_vec.push_back(ruleObject);
    }
}



