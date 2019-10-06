#ifndef _EXPLORE_H_
#define _EXPLORE_H_

class Node;

struct Explore {

    // Start exploration of the given node, using the cPUCT and
    //   the number of playouts given. The node must be fully
    //   initialized and ready to call playout() on. Displays
    //   statistics about the memory usage, the iteration number,
    //   and the current optimal path in a curses display.
    static void explore(Node* root, double cPUCT, long numPlayouts);
};

#endif
