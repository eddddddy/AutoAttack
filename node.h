#ifndef _NODE_H_
#define _NODE_H_

#include <string>
#include <memory>

#include "state.h"

struct NodeImpl;

class Node final {

    private:
        std::unique_ptr<NodeImpl> imp;

    public:
        Node();
        ~Node();
 
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
        //   the root to any leaf and the total dps of this path. The
        //   path is determined by starting at the root, and at each
        //   node taking the edge with the highest visit count (from all
        //   the playouts) until a leaf node is reached. The string is
        //   constructed by concatenating the string representations of
        //   the skills in the edges of this path.
        std::pair<std::string, double> currentBestPath();
};

#endif
