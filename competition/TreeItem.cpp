//
//  TreeItem.cpp
//  mancala_competition
//
//  Created by Hunter McNichols on 11/9/15.
//  Copyright © 2015 hunter_mc. All rights reserved.
//

#include "TreeItem.h"
#include <string>

void TreeItem::print_to_log(std::ofstream &log, int alpha, int beta){
    std::string s_alpha;
    std::string s_beta;
    if(alpha == INT32_MAX){
        s_alpha = "Infinity";
    }
    else if(alpha == INT32_MIN){
        s_alpha = "-Infinity";
    }
    else{
        std::ostringstream s;
        s << alpha;
        std::string convert(s.str());
        s_alpha = convert;
    }
    if(beta == INT32_MAX){
        s_beta = "Infinity";
    }
    else if(beta == INT32_MIN){
        s_beta = "-Infinity";
    }
    else{
        std::ostringstream s;
        s << beta;
        std::string convert(s.str());
        s_beta = convert;
    }
    
    if(value == INT32_MAX){
        log << move_name << "," << depth << ",Infinity," << s_alpha << "," << s_beta << std::endl;
        //std::cout << move_name << "," << depth << ",Infinity," << s_alpha << "," << s_beta << std::endl;
    }
    else if(value == INT32_MIN){
        log << move_name << "," << depth << ",-Infinity," << s_alpha << "," << s_beta << std::endl;
        //std::cout << move_name << "," << depth << ",-Infinity," << s_alpha << "," << s_beta << std::endl;
    }
    else{
        log << move_name << "," << depth << "," << value << "," << s_alpha << "," << s_beta << std::endl;
        //std::cout << move_name << "," << depth << "," << value << "," << s_alpha << "," << s_beta << std::endl;
    }
}

void TreeItem::print_to_log(std::ofstream &log, bool print){
    if(!print){
        return;
    }
    if(value == INT32_MAX){
        log << move_name << "," << depth << ",Infinity" << std::endl;
        //std::cout << move_name << "," << depth << ",Infinity" << std::endl;
    }
    else if(value == INT32_MIN){
        log << move_name << "," << depth << ",-Infinity" << std::endl;
        //std::cout << move_name << "," << depth << ",-Infinity" << std::endl;
    }
    else{
        log << move_name << "," << depth << "," << value << std::endl;
        //std::cout << move_name << "," << depth << "," << value << std::endl;
    }
}

TreeItem::TreeItem(std::string name) : move_name(name){
    value = INT32_MIN;
    max = true;//Root is always a max
    next_move = NULL;
    player_num = 0;
    move_index = 0;
    depth = 0;//root
    bonus = false;
}

TreeItem::TreeItem(bool type, int value, int player_num, int cell_num, int depth):max(type), value(value), player_num(player_num), move_index(cell_num), depth(depth){
    next_move = NULL;
    if(player_num==1){
        std::ostringstream s;
        s << "B" << (cell_num + 2);
        std::string convert(s.str());
        move_name = convert;
    }
    else if(player_num==2){
        std::ostringstream s;
        s << "A" << (cell_num + 2);
        std::string convert(s.str());
        move_name = convert;
    }
    bonus = false;
}
