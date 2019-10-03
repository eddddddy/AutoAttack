#ifndef _NODE_H_
#define _NODE_H_

#include <string>
#include <vector>
#include <memory>

#include "state.h"

class Node final {

    private:
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

    public:

        // Set the current state, stealing ownership of the pointer
        //   passed in. This method must be called before any other
        //   method is called, otherwise behaviour is undefined. This
        //   method must be called only once, otherwise behaviour is
        //   also undefined.
        void setState(std::unique_ptr<State>&& state);

        // Performs a single iteration of a Monte-Carlo playout, using
        //   c as the value of cPUCT (the degree to which exploration
        //   is preferred). The tree rooted at this node increases in
        //   size by exactly one node.
        void playout(double c);

        // Get a string representing the current optimal path from
        //   the root to any leaf. The path is determined by starting
        //   at the root, and at each node taking the edge with the
        //   highest visit count (from all the playouts) until a leaf
        //   node is reached. The string is constructed by concatenating
        //   the string representations of the skills in the edges of
        //   this path.
        std::string currentBestPath();
};

#endif
