#include <fstream>
#include <iostream>
#include "InterpretJson.h"
#include <glog/logging.h>

using Json = nlohmann::json;

InterpretJson::InterpretJson(std::string game_name, User owner) 
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

std::string getTextToReplace(std::string str) {
    size_t open_brace = 0; 
    if ((open_brace = str.find("{", open_brace)) != std::string::npos) {
        size_t close_brace = str.find("}", open_brace);
        return str.substr(open_brace, close_brace - open_brace + 1);
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
            auto msgString = rule->getMapElement("value")->getString();
            std::shared_ptr<ASTNode> elementToReplace;
            expressionTree.build(getTextToReplace(msgString));
            elementToReplace = expressionTree.getRoot();

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
            
            expressionTree.build(getTextToReplace(prompt));
            auto elementToReplace = expressionTree.getRoot();
            

            auto choicesString = rule->getMapElement("choices")->getString();
            expressionTree.build(choicesString);
            auto choicesExpressionRoot = expressionTree.getRoot();

            auto result = rule->getMapElement("result")->getString();
            auto timeout = rule->getMapElement("timeout")->getInt();
            ruleObject = std::make_shared<InputChoice>(prompt, elementToReplace, choicesExpressionRoot,
                 game._input_requests, game._player_input, result, timeout);
        }

        else if(ruleName == "input-text"){
            auto prompt = rule->getMapElement("prompt")->getString();
 
            auto result = rule->getMapElement("result")->getString();
            auto timeout = rule->getMapElement("timeout")->getInt();
            ruleObject = std::make_shared<InputText>(prompt,
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

        else if (ruleName == "sort"){
            auto listString = rule->getMapElement("list")->getString();
            expressionTree.build(listString);
            auto listExpressionRoot = expressionTree.getRoot();
            auto key = rule->getMapElement("key");
            if (key != nullptr)
                ruleObject = std::make_shared<Sort>(listExpressionRoot, key->getString());
            else
                ruleObject = std::make_shared<Sort>(listExpressionRoot);
        }

        else if (ruleName == "shuffle"){
            auto listString = rule->getMapElement("list")->getString();
            expressionTree.build(listString);
            auto listExpressionRoot = expressionTree.getRoot();

            ruleObject = std::make_shared<Shuffle>(listExpressionRoot);
        }

        else if (ruleName == "deal"){
            auto fromString = rule->getMapElement("from")->getString();
            expressionTree.build(fromString);
            auto fromExpressionRoot = expressionTree.getRoot();
            auto toString = rule->getMapElement("to")->getString();
            expressionTree.build(toString);
            auto toExpressionRoot = expressionTree.getRoot();
            auto count = rule->getMapElement("count")->getInt();
            ruleObject = std::make_shared<Deal>(fromExpressionRoot, toExpressionRoot, count);
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

        else if(ruleName == "switch"){       
            auto listString = rule->getMapElement("list")->getString();
            expressionTree.build(listString);
            auto listExpressionRoot = expressionTree.getRoot();

            auto valueString = rule->getMapElement("value")->getString();
            expressionTree.build(valueString);
            auto valueExpressionRoot = expressionTree.getRoot();

            std::vector<std::pair<std::shared_ptr<ASTNode>, RuleVector>> conditionExpression_rule_pairs;
            ElementVector cases = rule->getMapElement("cases")->getVector();
            for(auto& caseRulePair : cases){
                auto conditionString = caseRulePair->getMapElement("case")->getString();
                expressionTree.build(conditionString);
                auto conditionExpressionRoot = expressionTree.getRoot();

                RuleVector caseRules;
                toRuleVec(game, caseRulePair->getMapElement("rules"), caseRules);
                conditionExpression_rule_pairs.push_back({conditionExpressionRoot, caseRules});
            }
            ruleObject = std::make_shared<Switch>(valueExpressionRoot, listExpressionRoot, conditionExpression_rule_pairs);
        }
        rule_vec.push_back(ruleObject);
    }
}


