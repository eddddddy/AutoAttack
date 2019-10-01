#include <memory>
#include <utility>

#include "skill.h"
#include "resources.h"
#include "node.h"
#include "edge.h"

Edge::Edge(Node* parent, double priorP, Skill* skill):
    N{0}, W{0}, Q{0}, P{priorP}, parent{parent}, skill{skill}, time{skill->getCastTime()} {}

Edge::Edge(Node* parent, double priorP, int waitTime):
    N{0}, W{0}, Q{0}, P{priorP}, parent{parent}, skill{nullptr}, time{waitTime} {}

Node* Edge::getParent() const {return parent;}

Skill* Edge::getSkill() const {return skill;}

int Edge::getTime() const {return time;}

void Edge::setChild(std::unique_ptr<Node>&& node) {
    child = std::move(node);
}

Node* Edge::getChild() const {return child.get();}

int Edge::getN() const {return N;}

double Edge::getQ() const {return Q;}

double Edge::getP() const {return P;}
        
void Edge::addValue(double value) {
    N++;
    W += value;
    Q = W / N;
}
