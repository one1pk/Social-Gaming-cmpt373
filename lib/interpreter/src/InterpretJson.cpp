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
            auto msgString = rule->getMapElement("value")->getString();
            std::shared_ptr<ASTNode> elementToReplace;
            extractFromBraces(msgString, elementToReplace);
            ruleObject = std::make_shared<GlobalMsg>(msgString, game._global_msgs, elementToReplace);
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
                auto conditionString = caseRulePair->getMapElement("condition")->getString();
                expressionTree.build(conditionString);
                auto conditionExpressionRoot = expressionTree.getRoot();

                RuleVector caseRules;
                toRuleVec(game, caseRulePair->getMapElement("rules"), caseRules);
                when->condition_rule_pairs.push_back({conditionExpressionRoot, caseRules});
            }
            when->set();
            ruleObject = when;
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
            ruleObject = std::make_shared<InputChoice>(prompt, elementToReplace, choicesExpressionRoot, timeout, result, game._player_msgs, game._player_input);
        }

        else if (ruleName == "add"){
            auto toString = rule->getMapElement("to")->getString();
            expressionTree.build(toString);
            auto toExpressionRoot = expressionTree.getRoot();

            auto value = rule->getMapElement("value");
            ruleObject = std::make_shared<Add>(toExpressionRoot, value);
        }

        else if (ruleName == "scores"){
            auto score = rule->getMapElement("score")->getString();
            auto ascending = rule->getMapElement("ascending")->getBool();
            ruleObject = std::make_shared<Scores>(game._players, score, ascending, game._global_msgs);
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

        else if (ruleName == "extend"){
            auto targetString = rule->getMapElement("target")->getString();
            expressionTree.build(targetString);
            auto targetExpressionRoot = expressionTree.getRoot();

            auto listString = rule->getMapElement("list")->getString();
            expressionTree.build(listString);
            auto listExpressionRoot = expressionTree.getRoot();
            ruleObject = std::make_shared<Extend>(targetExpressionRoot, listExpressionRoot);
        }

        rule_vec.push_back(ruleObject);
    }
}



