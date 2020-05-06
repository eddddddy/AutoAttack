#include <iostream>
#include <string>
#include <curses.h>

#include "node.h"
#include "memcheck.h"
#include "explore.h"

void Explore::explore(Node* root, double cPUCT, long numPlayouts) {

    initscr();
    noecho();

    move(0, 0); printw("---------------------------------------------------");
    move(1, 0); printw(("Total Virtual Memory: " + std::to_string(MemCheck::getTotalVirtualMem())).c_str());
    move(2, 0); printw(("Total Physical Memory: " + std::to_string(MemCheck::getTotalPhysicalMem())).c_str());
    move(3, 0); printw("---------------------------------------------------");
    move(4, 0); printw(("Virtual Memory In Use By Process: " + std::to_string(MemCheck::getProcessVirtualMem())).c_str());
    move(5, 0); printw(("Physical Memory In Use By Process: " + std::to_string(MemCheck::getProcessPhysicalMem())).c_str());
    move(6, 0); printw("---------------------------------------------------");    
    move(7, 0); printw("Iteration: 0");
    move(8, 0); printw("Theoretical DPS: ");
    move(9, 0); printw("Best Rotation: ");
    refresh();

    for (int i = 1; i <= numPlayouts; i++) {
        root->playout(cPUCT);
        if (i % 10000 == 0) {
            std::pair<std::string, double> pathAndDamage = root->currentBestPath();
            move(0, 0); printw("---------------------------------------------------");
            move(1, 0); printw(("Total Virtual Memory: " + std::to_string(MemCheck::getTotalVirtualMem())).c_str());
            move(2, 0); printw(("Total Physical Memory: " + std::to_string(MemCheck::getTotalPhysicalMem())).c_str());
            move(3, 0); printw("---------------------------------------------------");
            move(4, 0); printw(("Virtual Memory In Use By Process: " + std::to_string(MemCheck::getProcessVirtualMem())).c_str());
            move(5, 0); printw(("Physical Memory In Use By Process: " + std::to_string(MemCheck::getProcessPhysicalMem())).c_str());
            move(6, 0); printw("---------------------------------------------------");
            move(7, 0); printw(("Iteration: " + std::to_string(i)).c_str());
            move(8, 0); printw(("Theoretical DPS: " + std::to_string(pathAndDamage.second)).c_str());
            move(9, 0); printw(("Best Rotation: " + pathAndDamage.first).c_str());
            refresh();
        }
    }

    endwin();
}
