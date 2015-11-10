//
//  TreeItem.h
//  mancala_competition
//
//  Created by Hunter McNichols on 11/9/15.
//  Copyright © 2015 hunter_mc. All rights reserved.
//

#ifndef TreeItem_h
#define TreeItem_h

#include <stdio.h>
#include <sstream>
#include <fstream>
#include <vector>

struct TreeItem{
    bool max;//true=MAX false=MIN
    TreeItem *parent;
    TreeItem *next_move;//NULL if last in a series of moves
    std::string move_name;
    std::vector<TreeItem *> children;
    int value;
    int player_num;
    int move_index;
    int depth;
    bool bonus;
    
    TreeItem(bool type, int value, int player_num, int cell_num, int depth);
    TreeItem(std::string name);//used for root node
    
    void print_to_log(std::ofstream &log, bool print);//for minimax false if greedy
    void print_to_log(std::ofstream &log, int alpha, int beta);//used in alpha-beta
};

#endif /* TreeItem_h */
