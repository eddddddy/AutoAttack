#ifndef _NODE_H_
#define _NODE_H_

#include <vector>
#include <memory>

#include "state.h"

class Edge;

class Node final {

    private:
        State state;

        Edge* parent;
        std::vector<std::unique_ptr<Edge>> children;

        int Nb;

    public:
        void initChildren();

        // Performs a single iteration of a Monte-Carlo playout
        void playout(double c);
};

#endif
