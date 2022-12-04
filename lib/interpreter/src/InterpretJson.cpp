#include <fstream>
#include <iostream>
#include "InterpretJson.h"
#include <glog/logging.h>

using namespace std;
using Json = nlohmann::json;

InterpretJson::InterpretJson(string game_name) 
    : game_name(game_name) {
    try {
        ifstream f(PATH_TO_JSON + game_name + ".json");
        Json jData = Json::parse(f);
        f.close();
        data = jData;
    } catch (std::exception& e){
        LOG(ERROR) << "error reading file" << e.what() << endl;
    }
}

Game InterpretJson::interpret(Connection owner) {
    ElementMap game_state = {
        {"setup", data.at("configuration").get<ElementSptr>()},
        {"constants", data.at("constants").get<ElementSptr>()},
        {"variables", data.at("constants").get<ElementSptr>()}
    };

    Game game(
        game_name, owner, game_state,
        data.at("per-player").get<ElementSptr>(),
        data.at("per-audience").get<ElementSptr>()
    );

    std::shared_ptr<ASTNode> root;
    expressionTree = ExpressionTree(root, game._game_state, game._players);

    // This stores all the content of the rules as strings
    ElementSptr rule_structure;
    data.at("rules").get_to(rule_structure);
    // then convert ElementSptr to rule vector containing rule objects
    RuleVector rules;
    toRuleVec(game, rule_structure, rules);

    return game;
}

std::string getTextExpression(std::string str) {
    size_t open_brace = 0; 
    if ((open_brace = str.find("{", open_brace)) != std::string::npos) {
        size_t close_brace = str.find("}", open_brace);
        return str.substr(open_brace+1, close_brace-open_brace-1);
    }
    return "";
}

//Interpret Rules
void InterpretJson::toRuleVec(Game& game, const ElementSptr& rules_from_json, RuleVector& rule_vec) {

    for (auto rule: rules_from_json->getVector()){
        std::string ruleName = rule->getMapElement("rule")->getString();
        RuleSptr ruleObject;

        if(ruleName == "foreach"){
            std::string listString = rule->getMapElement("list")->getString();
            expressionTree.build(listString);
            auto listExpressionRoot = expressionTree.getRoot();

            RuleVector subRules;
            toRuleVec(game, rule->getMapElement("rules"), subRules);
            auto elementName = rule->getMapElement("element")->getString();
            ruleObject = std::make_shared<Foreach>(listExpressionRoot, elementName, subRules);
        }

        else if(ruleName == "global-message"){
            auto msg = rule->getMapElement("value")->getString();
            expressionTree.build(getTextExpression(msg));
            std::shared_ptr<ASTNode> textExpressionRoot = expressionTree.getRoot();
            ruleObject = std::make_shared<GlobalMsg>(msg, textExpressionRoot, game._global_msgs);
        }

        else if(ruleName == "parallelfor"){
            RuleVector subRules;
            toRuleVec(game, rule->getMapElement("rules"), subRules);
            auto elementName = rule->getMapElement("element")->getString();
            ruleObject = std::make_shared<ParallelFor>(game._players, subRules, elementName);
        }

        else if(ruleName == "input-choice"){
            std::string choicesString = rule->getMapElement("choices")->getString();
            expressionTree.build(choicesString);
            std::shared_ptr<ASTNode> choicesExpressionRoot = expressionTree.getRoot();

            auto prompt = rule->getMapElement("prompt")->getString();
            expressionTree.build(getTextExpression(prompt));
            std::shared_ptr<ASTNode> textExpressionRoot = expressionTree.getRoot();

            auto result = rule->getMapElement("result")->getString();
            auto timeout = rule->getMapElement("timeout")->getInt();
            ruleObject = std::make_shared<InputChoice>(
                prompt, textExpressionRoot, choicesExpressionRoot, 
                game._input_requests, game._player_input, result, timeout
            );
        }

        else if (ruleName == "add"){
            auto to = rule->getMapElement("to")->getString();
            expressionTree.build(to);
            std::shared_ptr<ASTNode> elementExpressionRoot = expressionTree.getRoot();

            auto value = rule->getMapElement("value")->getString();
            expressionTree.build(value);
            std::shared_ptr<ASTNode> valueExpressionRoot = expressionTree.getRoot();

            ruleObject = std::make_shared<Add>(elementExpressionRoot, valueExpressionRoot);
        }

        else if (ruleName == "scores"){
            auto score = rule->getMapElement("score")->getString();
            auto ascending = rule->getMapElement("ascending")->getBool();
            ruleObject = std::make_shared<Scores>(game._players, score, ascending, game._global_msgs);
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

        else if (ruleName == "discard"){
            std::string fromString = rule->getMapElement("from")->getString();
            expressionTree.build(fromString);
            std::shared_ptr<ASTNode> fromExpressionRoot = expressionTree.getRoot();

            std::string countString = rule->getMapElement("count")->getString();
            expressionTree.build(countString);
            std::shared_ptr<ASTNode> countExpressionRoot = expressionTree.getRoot();
            ruleObject = std::make_shared<Extend>(fromExpressionRoot, countExpressionRoot);
        }

        else if(ruleName == "when"){
            std::vector<std::pair<std::shared_ptr<ASTNode>, RuleVector>> conditionExpressionRulePairs;
            ElementVector cases = rule->getMapElement("cases")->getVector();
            for(auto caseRulePair : cases){
                std::string conditionString = caseRulePair->getMapElement("condition")->getString();
                expressionTree.build(conditionString);
                std::shared_ptr<ASTNode> conditionExpressionRoot = expressionTree.getRoot();

                RuleVector caseRules;
                toRuleVec(game, caseRulePair->getMapElement("rules"), caseRules);
                conditionExpressionRulePairs.push_back({conditionExpressionRoot, caseRules});
            }

            ruleObject = std::make_shared<When>(conditionExpressionRulePairs);
        }

        rule_vec.push_back(ruleObject);
    }
}



