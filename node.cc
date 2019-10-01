#include <cmath>
#include <vector>
#include <unordered_map>
#include <memory>
#include <utility>

#include "skill.h"
#include "resources.h"
#include "state.h"
#include "edge.h"
#include "node.h"

void Node::initChildren() {
    children.clear();
    std::vector<Skill*> availableSkills = state.getAvailableSkills();
    for (Skill* skill : availableSkills) {
        children.emplace_back(std::make_unique<Edge>(this, static_cast<double>(skill->getDamage()) / skill->getCastTime(), skill));
    }
    children.emplace_back(std::make_unique<Edge>(this, 0, state.getWaitTime()));
}

void Node::playout(double c) {
    if (children.empty()) initChildren();

    // Selection phase
    Node* currNode = this;
    Edge* edgeToTake = nullptr;
    double maxEdgeValue = -1;
    do {
        maxEdgeValue = -1;
        for (auto it = currNode->children.begin(); it != currNode->children.end(); ++it) {
            Edge* thisEdge = (*it).get();
            double thisEdgeValue = thisEdge->getQ() + c * thisEdge->getP() * sqrt(Nb) / (1 + thisEdge->getN());
            if (thisEdgeValue > maxEdgeValue) {
                edgeToTake = thisEdge;
                maxEdgeValue = thisEdgeValue;
            }
        }
        currNode = edgeToTake->getChild();
    } while (currNode);

    // Expansion phase
    std::unique_ptr<Node> newNode = std::make_unique<Node>();
    std::unordered_map<Skill*, Skill*> oldToNew;
    newNode->state = state.copy(oldToNew);
    newNode->state.useSkill(oldToNew[edgeToTake->getSkill()]);
    newNode->parent = edgeToTake;
    newNode->initChildren();
    newNode->Nb = 0;
    edgeToTake->setChild(std::move(newNode));

    // Backpropagation phase
    Edge* currEdge = edgeToTake;
    int accumDamage = 0, accumTime = 0;
    do {
        accumDamage += (currEdge->getSkill() ? currEdge->getSkill()->getDamage() : 0);
        accumTime += currEdge->getTime();
        double dps = static_cast<double>(accumDamage) / accumTime;
        currEdge->addValue(dps);
        (currEdge->getParent()->Nb)++;
        currEdge = currEdge->getParent()->parent;
    } while (currEdge);
}
