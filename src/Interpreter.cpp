//Include of the required programs
#include "../include/Interpreter.hpp"
#include <stdexcept>

Interpreter::Interpreter(const ProgramNode* program, Environment env)
    :program(program), env(env) {}

void Interpreter::executeCycle(){
    if(program == false){
        return;
    }

    bool stateChanged;

    //Fixed point cycle
    do{

        stateChanged = false;

        //usage of a temporal set to collect the activations back ensuring determinism
        std::set<std::string> factsToActivateThisRound;

        //Evaluate all the rules
        for(const auto& rule : program -> rules){
            //If the condition of the rule is true
            if(evaluateCondition(rule->condition.get())){
                //Agregate the action to our collection list
                factsToActivateThisRound.insert(rule -> action -> factId);
            }
        }

        //Apply recollections in the start state
        for(const auto& fact : factsToActivateThisRound) {
            //ActivateFact returns true if the fact didnt exist and was succesfully inserted
            if(env.activateFact(fact)){
                newlyActivatedFacts.insert(fact);//Saved for the final output
                stateChanged = true; //The state changed there will be another one
            }
        }

        //Repeat until there are no changes
    } 
    while (stateChanged);
}

std::set<std::string> Interpreter::getActivatedFacts() const{
    return newlyActivatedFacts;
}

bool Interpreter::evaluateCondition(const ConditionNode* cond) const {
    if(cond == false){
        return false; 
    }
    
    //Scenario 1: Is a comparison
    if (auto cmp = dynamic_cast<const ComparisonNode* > (cond)){
        //Obtain the values from the env
        int variableValue = 0;
        if(env.hasVariable(cmp -> id)){
            variableValue = env.getVariable(cmp -> id);
        }
        else{
            //In case the variable does not exist we assume it false
            return false;
        }

        //Evaluate according to the operator
        if(cmp -> operation == TokenType::GREATER) {
            return variableValue > cmp -> value;
        }
        else if(cmp -> operation == TokenType::LESS) {
            return variableValue < cmp -> value;
        }
        else if(cmp -> operation == TokenType::EQUAL){
            return variableValue == cmp -> value; 
        }
    }
    //Scenario 2: Is a fact 
    else if(auto fact = dynamic_cast<const FactConditionNode*>(cond)){
        //Will only be true if the fact is inside the true fact list
        return env.isFactActive(fact -> factId);
    }
    //Scenario 3: Is a condition AND
    else if(auto andCond = dynamic_cast<const AndConditionNode*>(cond)){
        // Both righ and left conditions must be true
        return evaluateCondition(andCond -> leftCondition.get()) && evaluateCondition(andCond -> rightCondition.get());
    }
    
    return false;
}