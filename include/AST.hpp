//Pragma for eviting reading the file more than once
#pragma once

//Include of required files
#include "Token.hpp"

//Include of other required tools
#include <string>
#include <vector>
#include <memory>


//Base class
/*All nodes will inherit this class and will
use the destroyer for a clean management of memory*/
struct ASTNode {
    virtual ~ASTNode() = default;
};

//Condition class for polimorphism
struct conditionNode : public ASTNode {
    virtual ~conditionNode() = default;
};

//Atomical condition
