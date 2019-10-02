#ifndef _NODE_H_
#define _NODE_H_

#include <string>
#include <vector>
#include <memory>

#include "state.h"
#include "edge.h"

class Node final {

    private:
        std::unique_ptr<State> state;

        Edge* parent = nullptr;
        std::vector<std::unique_ptr<Edge>> children;

        int Nb = 0;

    public:
        // steals ownership
        void setState(std::unique_ptr<State>&& state);

        void initChildren();

        // Performs a single iteration of a Monte-Carlo playout
        void playout(double c);

        std::string currentBestPath();
};

#endif
