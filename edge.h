#ifndef _EDGE_H_
#define _EDGE_H_

#include <memory>

class Skill;
class Node;

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

#endif
