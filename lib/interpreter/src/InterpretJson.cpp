#include <fstream>
#include <iostream>
#include "InterpretJson.h"
#include <glog/logging.h>

using Json = nlohmann::json;

InterpretJson::InterpretJson(std::string game_name, Connection owner) 
    : game_name(game_name), owner(owner) {
    try {
        ifstream f(PATH_TO_JSON + game_name + ".json");
        Json jData = Json::parse(f);
        f.close();
        data = jData;
    } catch (std::exception& e){
        LOG(ERROR) << "error reading file" << e.what() << endl;
    }
}

InterpretJson::InterpretJson(std::string path) 
    {
    try {
        ifstream f(path);
        Json jData = Json::parse(f);
        f.close();
        data = jData;
    } catch (std::exception& e){
        LOG(ERROR) << "error reading file" << e.what() << endl;
    }
}

Game InterpretJson::interpret() {
    Game game = data.get<Game>();
    game.setID();
    game.setOwner(owner);
    game.setName(game_name);
    game._game_state = {
        {"constants", game.constants()},
        {"variables", game.variables()},
        {"setup", game.setup()},
        {"per-player", game.per_player()},
        {"per-audience", game.per_audience()}
    };

    expressionTree = ExpressionTree(game._game_state, game._players);

    // This stores all the content of the rules as strings
    ElementSptr rule_structure;
    data.at("rules").get_to(rule_structure);
    // then convert ElementSptr to rule vector containing rule objects
    RuleVector rules;
    toRuleVec(game, rule_structure, rules);

    game._rules = rules;
    return game;
}

std::string getTextExpression(std::string str) {
    size_t open_brace = 0; 
    if ((open_brace = str.find("{", open_brace)) != std::string::npos) {
        size_t close_brace = str.find("}", open_brace);
        return str.substr(open_brace+1, close_brace - open_brace + 1);
    }
    return "";
}

void InterpretJson::extractFromBraces(std::string msg, std::shared_ptr<ASTNode>& elementToReplace) {
    size_t openBrace = 0; 
        if ((openBrace = msg.find("{", openBrace)) != std::string::npos) {
            size_t closeBrace = msg.find("}", openBrace);
            std::string elementToReplaceString = msg.substr(openBrace, closeBrace - openBrace + 1);
            expressionTree.build(elementToReplaceString);
            elementToReplace = expressionTree.getRoot();
        }
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
            auto msgString = rule->getMapElement("value")->getString();
            std::shared_ptr<ASTNode> elementToReplace;
            extractFromBraces(msgString, elementToReplace);
            ruleObject = std::make_shared<GlobalMsg>(msgString, elementToReplace, game._global_msgs);
        }

        else if(ruleName == "parallelfor"){
            RuleVector subRules;
            toRuleVec(game, rule->getMapElement("rules"), subRules);
            auto elementName = rule->getMapElement("element")->getString();
            ruleObject = std::make_shared<ParallelFor>(game._players, subRules, elementName);
        }

        else if(ruleName == "input-choice"){
            auto prompt = rule->getMapElement("prompt")->getString();
            std::shared_ptr<ASTNode> elementToReplace;
            extractFromBraces(prompt, elementToReplace);

            auto choicesString = rule->getMapElement("choices")->getString();
            expressionTree.build(choicesString);
            auto choicesExpressionRoot = expressionTree.getRoot();

            auto result = rule->getMapElement("result")->getString();
            auto timeout = rule->getMapElement("timeout")->getInt();
            ruleObject = std::make_shared<InputChoice>(prompt, elementToReplace, choicesExpressionRoot,
                 game._input_requests, game._player_input, result, timeout);
        }

        else if (ruleName == "add"){
            auto to = rule->getMapElement("to")->getString();
            expressionTree.build(to);
            auto elementExpressionRoot = expressionTree.getRoot();

            auto value = rule->getMapElement("value")->getString();
            expressionTree.build(value);
            auto valueExpressionRoot = expressionTree.getRoot();
            ruleObject = std::make_shared<Add>(elementExpressionRoot, valueExpressionRoot);
        }

        else if (ruleName == "scores"){
            auto score = rule->getMapElement("score")->getString();
            auto ascending = rule->getMapElement("ascending")->getBool();
            ruleObject = std::make_shared<Scores>(game._players, score, ascending, game._global_msgs);
        }

        else if (ruleName == "extend"){
            auto targetString = rule->getMapElement("target")->getString();
            expressionTree.build(targetString);
            auto targetExpressionRoot = expressionTree.getRoot();

            auto listString = rule->getMapElement("list")->getString();
            expressionTree.build(listString);
            auto listExpressionRoot = expressionTree.getRoot();
            ruleObject = std::make_shared<Extend>(targetExpressionRoot, listExpressionRoot);
        }

        else if (ruleName == "discard"){
            auto fromString = rule->getMapElement("from")->getString();
            expressionTree.build(fromString);
            auto fromExpressionRoot = expressionTree.getRoot();

            auto countString = rule->getMapElement("count")->getString();
            expressionTree.build(countString);
            auto countExpressionRoot = expressionTree.getRoot();
            ruleObject = std::make_shared<Discard>(fromExpressionRoot, countExpressionRoot);
        }

        else if(ruleName == "when"){            
            std::vector<std::pair<std::shared_ptr<ASTNode>, RuleVector>> conditionExpression_rule_pairs;
            ElementVector cases = rule->getMapElement("cases")->getVector();
            for(auto& caseRulePair : cases){
                auto conditionString = caseRulePair->getMapElement("condition")->getString();
                expressionTree.build(conditionString);
                auto conditionExpressionRoot = expressionTree.getRoot();

                RuleVector caseRules;
                toRuleVec(game, caseRulePair->getMapElement("rules"), caseRules);
                conditionExpression_rule_pairs.push_back({conditionExpressionRoot, caseRules});
            }
            ruleObject = std::make_shared<When>(conditionExpression_rule_pairs);
        }

        rule_vec.push_back(ruleObject);
    }
}


