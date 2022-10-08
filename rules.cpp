#include "rules.h"


/****************** CONTROL STRUCTURES ********************/

/*** Foreach ***/

Foreach::Foreach(List _list, std::vector<Rule> _rules) 
    : list(_list), rules(_rules) {}

void Foreach::execute () {

}

/*** Loop ***/

Loop::Loop(bool _until, std::vector<Rule> _rules)
    : rules(_rules) {}

void Loop::execute () {

}

/*** Inparallel ***/

Inparallel::Inparallel(std::vector<Rule> _rules)
    : rules(_rules) {}

void Inparallel::execute () {

}

/*** Parallelfor ***/

Parallelfor::Parallelfor(List _list, std::vector<Rule> _rules)
    : list(_list), rules(_rules) {}

void Parallelfor::execute () {

}

/*** Switch ***/

Switch::Switch(std::any _value, List _list, std::vector<std::pair<std::any,std::vector<Rule>>> _case_rules)
    : value(_value), list(_list), case_rules(_case_rules) {}

void Switch::execute () {

}

/*** When ***/

When::When(std::any _value, std::vector<std::pair<std::any,std::vector<Rule>>> _case_rules)
    : value(_value), case_rules(_case_rules) {}

void When::execute () {

}

/***************** ARITHMETIC OPERATIONS ******************/

/*** Add ***/

Add::Add(int &_to, int _value)
    : to(_to), value(_value) {}

void Add::execute () {

}

/*** Numerical Attributes ***/

Countup::Countup(int _value)
    : value(_value) {}

void Countup::execute () {

}

/************************* TIMING *************************/

/*** Timer ***/


/********************** HUMAN INPUT ***********************/

/*** Input Choice ***/

/*** Input Text ***/

/*** Input Vote ***/

/************************* OUTPUT *************************/

/*** Message ***/

/*** Global Message ***/

/*** Scores ***/
