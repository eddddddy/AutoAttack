#include <cmath>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <utility>

#include "skill.h"
#include "resources.h"
#include "state.h"
#include "node.h"

struct NodeImpl {

    Node* owner;

    class Edge final {
        private:
            int N;
            double W, Q;
            const double P;

            Node* parent;
            std::unique_ptr<Node> child;

            Skill* skill;
            int time;

        public:
            Edge(Node* parent, double priorP, Skill* skill);
            Edge(Node* parent, double priorP, int waitTime);

            Node* getParent() const;

            Skill* getSkill() const;

            int getTime() const;

            void setChild(std::unique_ptr<Node>&& node);
            Node* getChild() const;

            int getN() const;
            double getQ() const;
            double getP() const;

            void addValue(double value);
    };

    std::unique_ptr<State> state;

    Edge* parent = nullptr;
    std::vector<std::unique_ptr<Edge>> children;

    int Nb = 0;

    void initChildren();
};

NodeImpl::Edge::Edge(Node* parent, double priorP, Skill* skill):
    N{0}, W{0}, Q{0}, P{priorP}, parent{parent}, child{std::unique_ptr<Node>()}, skill{skill}, time{skill->getCastTime()} {}

NodeImpl::Edge::Edge(Node* parent, double priorP, int waitTime):
    N{0}, W{0}, Q{0}, P{priorP}, parent{parent}, child{std::unique_ptr<Node>()}, skill{nullptr}, time{waitTime} {}

Node* NodeImpl::Edge::getParent() const {return parent;}

Skill* NodeImpl::Edge::getSkill() const {return skill;}

int NodeImpl::Edge::getTime() const {return time;}

void NodeImpl::Edge::setChild(std::unique_ptr<Node>&& node) {
    child = std::move(node);
}

Node* NodeImpl::Edge::getChild() const {return child ? child.get() : nullptr;}

int NodeImpl::Edge::getN() const {return N;}

double NodeImpl::Edge::getQ() const {return Q;}

double NodeImpl::Edge::getP() const {return P;}

void NodeImpl::Edge::addValue(double value) {
    N++;
    W += value;
    Q = W / N;
}

Node::Node(): imp{std::make_unique<NodeImpl>()} {imp->owner = this;}

Node::~Node() = default;

void NodeImpl::initChildren() {
    children.clear();
    std::vector<Skill*> availableSkills = state->getAvailableSkills();
    for (Skill* skill : availableSkills) {
        children.emplace_back(std::make_unique<Edge>(owner, static_cast<double>(skill->getDamage()) / skill->getCastTime(), skill));
    }
    children.emplace_back(std::make_unique<Edge>(owner, 0, state->getWaitTime()));
}

void Node::setState(std::unique_ptr<State>&& state) {
    this->imp->state = std::move(state);
}

void Node::playout(double c) {
    if (imp->children.empty()) imp->initChildren();

    // Selection phase
    Node* currNode = this;
    NodeImpl::Edge* edgeToTake = nullptr;
    double maxEdgeValue = -1;
    do {
        maxEdgeValue = -1;
        for (auto it = currNode->imp->children.begin(); it != currNode->imp->children.end(); ++it) {
            NodeImpl::Edge* thisEdge = (*it).get();
            double thisEdgeValue = thisEdge->getQ() + c * thisEdge->getP() * sqrt(imp->Nb) / (1 + thisEdge->getN());
            if (thisEdgeValue > maxEdgeValue) {
                edgeToTake = thisEdge;
                maxEdgeValue = thisEdgeValue;
            }
        }
        currNode = edgeToTake->getChild();
    } while (currNode);

    // Expansion phase
    currNode = edgeToTake->getParent();
    std::unique_ptr<Node> newNode = std::make_unique<Node>();
    std::unordered_map<Skill*, Skill*> oldToNew;
    newNode->imp->state = std::unique_ptr<State>(currNode->imp->state->copy(oldToNew));
    newNode->imp->state->useSkill(oldToNew[edgeToTake->getSkill()], edgeToTake->getTime());
    newNode->imp->parent = edgeToTake;
    newNode->imp->initChildren();
    newNode->imp->Nb = 0;
    edgeToTake->setChild(std::move(newNode));

    // Backpropagation phase
    NodeImpl::Edge* currEdge = edgeToTake;
    int accumDamage = 0, accumTime = 0;
    do {
        accumDamage += (currEdge->getSkill() ? currEdge->getSkill()->getDamage() : 0);
        accumTime += currEdge->getTime();
        double dps = static_cast<double>(accumDamage) / accumTime;
        currEdge->addValue(dps);
        (currEdge->getParent()->imp->Nb)++;
        currEdge = currEdge->getParent()->imp->parent;
    } while (currEdge);
}

std::string Node::currentBestPath() {
    if (imp->children.empty()) imp->initChildren();

    std::string path = "";
    Node* currNode = this;
    NodeImpl::Edge* edgeToTake = nullptr;
    double maxEdgeVisits = -1;
    do {
        maxEdgeVisits = -1;
        for (auto it = currNode->imp->children.begin(); it != currNode->imp->children.end(); ++it) {
            NodeImpl::Edge* thisEdge = (*it).get();
            if (thisEdge->getN() > maxEdgeVisits) {
                edgeToTake = thisEdge;
                maxEdgeVisits = thisEdge->getN();
            }
        }
        if (edgeToTake->getSkill()) path += edgeToTake->getSkill()->toString() + " ";
        currNode = edgeToTake->getChild();
    } while (currNode);

    return path;
}
