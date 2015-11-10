//
//  main.cpp
//  mancala_competition
//
//  Created by Hunter McNichols on 11/9/15.
//  Copyright © 2015 hunter_mc. All rights reserved.
//
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <climits>
#include <vector>

#include "MancalaBoard.h"
#include "TreeItem.h"
#include "GameTree.h"

class MancalaBoard;
struct TreeItem;
class GameTree;

void run_greedy(int p, std::vector<int>&p2_state, std::vector<int>&p1_state, int p2S, int p1S, std::ofstream &next_state);
void run_minimax(int p, std::vector<int>&p2_state, std::vector<int>&p1_state, int p2S, int p1S, int depth, std::ofstream &next_state, std::ofstream &log);
void run_ab(int p, std::vector<int>&p2_state, std::vector<int>&p1_state, int p2S, int p1S, int depth, std::ofstream &output, std::ofstream &log);
TreeItem *minimax_value(MancalaBoard& board, int current_depth, int max_depth, int player_num, bool is_max, TreeItem *node, int root_player, bool print, std::ofstream &log, GameTree &g);
TreeItem *minimax_value_ab(MancalaBoard& board, int current_depth, int max_depth, int player_num, bool is_max, TreeItem* cur_node, int root_player, std::ofstream &log, int alpha, int beta, GameTree &g);


int main(int argc, char *argv[]){
    /* Input file format:
     <Task#> Greedy=1, MiniMax=2, Alpha-Beta=3, Competition=4
     <Your player: 1 or 2>
     <Cutting off depth>
     <Board state for player-2>
     <Board state for player-1>
     <#stones in player-2’s mancala>
     <#stones in player-1’s mancala>
     */
    
    /* Input file for competition
     <task = 4 for competition>
     <Your player: 1 or 2>
     <your CPU time left in seconds: floating point number>
     <Board state for player-2>
     <Board state for player-1>
     <#stones in player-2’s mancala>
     <#stones in player-1’s mancala>
     */
    
    int task_num;
    int player_num;
    int cutoff;
    //top player is player 2, bottom is player 1
    std::vector<int> top_player;
    std::vector<int> bot_player;
    int p1_score;
    int p2_score;
    
//    if(argc < 2){
//        std::cout << "Fail, not enough args" << std::endl;
//        return 1;
//    }
    
//    std::ifstream input_file(argv[2]);
    std::ifstream input_file("input_4.txt");
    if(!input_file.is_open()){
        std::cout << "File not found" << std::endl;
        return 1;
    }
    //Output stream
    std::ofstream next_state("next_state.txt");
    input_file >> task_num;
    input_file.ignore(INT_MAX, '\n');
    input_file >> player_num;
    input_file.ignore(INT_MAX, '\n');
    input_file >> cutoff;
    input_file.ignore(INT_MAX, '\n');
    
    std::string line;
    int count = 0;
    if(std::getline(input_file, line)){
        std::istringstream iss(line);
        int stones;
        while(iss >> stones){
            top_player.push_back(stones);
            count++;
        }
    }
    
    if(std::getline(input_file, line)){
        std::istringstream iss(line);
        int stones;
        while(iss >> stones){
            bot_player.push_back(stones);
        }
    }
    
    input_file >> p2_score;
    input_file.ignore(INT_MAX, '\n');
    input_file >> p1_score;
    input_file.ignore(INT_MAX, '\n');
    
    if(task_num == 1){
        run_greedy(player_num, top_player, bot_player, p2_score, p1_score, next_state);
    }
    else if(task_num == 2){
        std::ofstream log("traverse_log.txt");
        run_minimax(player_num, top_player, bot_player, p2_score, p1_score, cutoff, next_state, log);
    }
    else if(task_num == 3){
        std::ofstream log("traverse_log.txt");
        run_ab(player_num, top_player, bot_player, p2_score, p1_score, cutoff, next_state, log);
    }
    else if (task_num == 4){
        std::cout << "Haven't written competition code yet" << std::endl;
    }
    else{
        std::cout << "Invalid task code" << std::endl;
    }
    
    
    return 0;
}

//Algorithms
void run_greedy(int p, std::vector<int>&p2_state, std::vector<int>&p1_state, int p2S, int p1S, std::ofstream &next_state){
    run_minimax(p, p2_state, p1_state, p2S, p1S, 1, next_state, next_state);
}

void run_minimax(int p, std::vector<int>&p2_state, std::vector<int>&p1_state, int p2S, int p1S, int depth, std::ofstream &next_state, std::ofstream &log){
    bool print = true;
    if(&next_state == &log){
        print = false;//greedy scenario
    }
    else{
        //std::cout << "Node,Depth,Value" << std::endl;
        log << "Node,Depth,Value" << std::endl;
    }
    //TODO clean up all the TreeItem memory
    GameTree tree = GameTree();
    TreeItem *root = new TreeItem("root");
    tree.addChild(NULL, root);
    tree.addNode(root);
    
    TreeItem *best_move = root;
    MancalaBoard initialBoard = MancalaBoard(p2_state, p1_state, p2S, p1S);
    
    int *next_moves = initialBoard.next_moves(p);
    int count = initialBoard.moves_available(p);
    root->print_to_log(log, print);
    for (int i=0; i < count; i++) {
        MancalaBoard traverseBoard = initialBoard;
        bool bonus = traverseBoard.make_move(p, next_moves[i]);
        int cur_depth = 1;
        bool is_max = bonus;
        int next_player = p;
        if(!bonus){
            if(p == 1){
                next_player = 2;
            }
            else{
                next_player = 1;
            }
        }
        TreeItem *next_node;
        if(is_max){
            next_node = new TreeItem(is_max, INT32_MIN, p, next_moves[i], cur_depth);
            tree.addNode(next_node);
        }
        else{
            next_node = new TreeItem(is_max, INT32_MAX, p, next_moves[i], cur_depth);
            tree.addNode(next_node);
        }
        next_node->bonus = bonus;
        
        TreeItem *best_child = minimax_value(traverseBoard, cur_depth, depth, next_player, is_max, next_node, p, print, log, tree);
        if(bonus){
            next_node->next_move = best_child;
        }
        next_node->value = best_child->value;
        if(next_node->value > best_move->value){
            best_move = next_node;
            root->value = best_move->value;
        }
        root->print_to_log(log, print);
    }
    delete [] next_moves;
    //at this point best_move should correspond to the TreeItem of the best move
    
    //Make the series of moves to update the board to the new state
    initialBoard.make_move(best_move->player_num, best_move->move_index);
    TreeItem *move = best_move->next_move;
    while(move != NULL){
        initialBoard.make_move(move->player_num, move->move_index);
        move = move->next_move;
    }
    initialBoard.checkGameOver(best_move->player_num);
    initialBoard.print_to_file(next_state);
    
    
    //delete best_move;
    //Clear up memory made by the tree!
}

TreeItem *minimax_value(MancalaBoard& board, int current_depth, int max_depth, int player_num, bool is_max, TreeItem* cur_node, int root_player, bool print, std::ofstream &log, GameTree &g){
    //if terminal test/ depth test
    if (!cur_node->bonus && (board.moves_available(player_num) == 0 || current_depth >= max_depth) ){
        cur_node->value = board.util_value(player_num, root_player);
        cur_node->print_to_log(log, print);
        return cur_node;
    }
    //else if MAX's move
    else if(is_max){
        //return highest minimax_value of successors
        cur_node->print_to_log(log, print);
        TreeItem *max_child_node = cur_node;
        int *next_moves = board.next_moves(player_num);
        int count = board.moves_available(player_num);
        for (int i=0; i<count; i++) {
            MancalaBoard nextBoard = MancalaBoard(board);
            bool bonus = nextBoard.make_move(player_num, next_moves[i]);
            int depth = cur_node->bonus ? current_depth : current_depth + 1;
            bool is_max = bonus;//stays true if a bonus move
            int next_player = player_num;
            if(!bonus){
                if(player_num == 1){
                    next_player = 2;
                }
                else{
                    next_player = 1;
                }
            }
            
            TreeItem *next_node;
            if(is_max){
                next_node = new TreeItem(is_max, INT32_MIN, player_num, next_moves[i], depth);
                g.addNode(next_node);
            }
            else{
                next_node = new TreeItem(is_max, INT32_MAX, player_num, next_moves[i], depth);
                g.addNode(next_node);
            }
            next_node->bonus = bonus;
            
            
            TreeItem *best_child = minimax_value(nextBoard, depth, max_depth, next_player, is_max, next_node, root_player, print, log, g);
            if(bonus){//if it's a part of a string of moves
                next_node->next_move = best_child;
            }
            
            if(!max_child_node){
                max_child_node = next_node;
                cur_node->value = max_child_node->value;
            }
            else if(next_node->value > max_child_node->value){
                max_child_node = next_node;
                cur_node->value = max_child_node->value;
            }
            cur_node->print_to_log(log, print);
        }
        delete [] next_moves;
        return max_child_node;
    }
    //else (i.e. MIN's move)
    else{
        //return lowest minimax_value of successors
        cur_node->print_to_log(log, print);
        TreeItem *min_child_node = cur_node;
        int *next_moves = board.next_moves(player_num);
        int count = board.moves_available(player_num);
        for (int i=0; i<count; i++) {
            MancalaBoard nextBoard = MancalaBoard(board);
            bool bonus = nextBoard.make_move(player_num, next_moves[i]);
            int depth = cur_node->bonus ? current_depth : current_depth + 1;
            bool is_max = !bonus;//MIN && bonus -> false,  MIN && !bonus -> true
            int next_player = player_num;
            if(!bonus){
                if(player_num == 1){
                    next_player = 2;
                }
                else{
                    next_player = 1;
                }
            }
            
            TreeItem *next_node;
            if(is_max){
                next_node = new TreeItem(is_max, INT32_MIN, player_num, next_moves[i], depth);
                g.addNode(next_node);
            }
            else{
                next_node = new TreeItem(is_max, INT32_MAX, player_num, next_moves[i], depth);
                g.addNode(next_node);
            }
            next_node->bonus = bonus;
            
            //next_node->print_to_log(log, print);
            TreeItem *best_child = minimax_value(nextBoard, depth, max_depth, next_player, is_max, next_node, root_player, print, log, g);
            if(bonus){//if it's a part of a string of moves
                next_node->next_move = best_child;
            }
            next_node->value = best_child->value;
            if(!min_child_node){
                min_child_node = next_node;
                cur_node->value = min_child_node->value;
            }
            else if(next_node->value < min_child_node->value){
                min_child_node = next_node;
                cur_node->value = min_child_node->value;
            }
            
            cur_node -> print_to_log(log, print);
        }
        delete [] next_moves;
        return min_child_node;
    }
}
void run_ab(int p, std::vector<int>&p2_state, std::vector<int>&p1_state, int p2S, int p1S, int depth, std::ofstream &next_state, std::ofstream &log){
    
    //std::cout << "Node,Depth,Value,Alpha,Beta" << std::endl;
    log << "Node,Depth,Value,Alpha,Beta" << std::endl;
    
    //TODO clean up all the TreeItem memory
    GameTree tree = GameTree();
    TreeItem *root = new TreeItem("root");
    tree.addChild(NULL, root);
    tree.addNode(root);
    
    TreeItem *best_move = root;
    MancalaBoard initialBoard = MancalaBoard(p2_state, p1_state, p2S, p1S);
    
    int *next_moves = initialBoard.next_moves(p);
    int count = initialBoard.moves_available(p);
    int alpha = INT32_MIN;
    int beta = INT32_MAX;
    root->print_to_log(log, alpha, beta);
    for (int i=0; i < count; i++) {
        MancalaBoard traverseBoard = initialBoard;
        bool bonus = traverseBoard.make_move(p, next_moves[i]);
        int cur_depth = 1;
        bool is_max = bonus;
        int next_player = p;
        if(!bonus){
            if(p == 1){
                next_player = 2;
            }
            else{
                next_player = 1;
            }
        }
        TreeItem *next_node;
        if(is_max){
            next_node = new TreeItem(is_max, INT32_MIN, p, next_moves[i], cur_depth);
            tree.addNode(next_node);
        }
        else{
            next_node = new TreeItem(is_max, INT32_MAX, p, next_moves[i], cur_depth);
            tree.addNode(next_node);
        }
        next_node->bonus = bonus;
        TreeItem *best_child = minimax_value_ab(traverseBoard, cur_depth, depth, next_player, is_max, next_node, p, log, alpha, beta, tree);
        if(bonus){//if it's a part of a string of moves, needs fixing
            next_node->next_move = best_child;
        }
        next_node->value = best_child->value;
        if(next_node->value > best_move->value){
            best_move = next_node;
            root->value = best_move->value;
        }
        if(best_move->value >= beta){
            break;//found! this should never hit
        }
        if(best_move->value > alpha){
            alpha = best_move->value;
        }
        
        root->print_to_log(log, alpha, beta);
    }
    delete [] next_moves;
    //at this point best_move should correspond to the TreeItem of the best move
    
    //Make the series of moves to update the board to the new state
    initialBoard.make_move(best_move->player_num, best_move->move_index);
    TreeItem *move = best_move->next_move;
    while(move != NULL){
        initialBoard.make_move(move->player_num, move->move_index);
        move = move->next_move;
    }
    
    initialBoard.print_to_file(next_state);
}

TreeItem *minimax_value_ab(MancalaBoard& board, int current_depth, int max_depth, int player_num, bool is_max, TreeItem* cur_node, int root_player, std::ofstream &log, int alpha, int beta, GameTree &g){
    if (!cur_node->bonus && (board.moves_available(player_num) == 0 || current_depth >= max_depth) ){
        cur_node->value = board.util_value(player_num, root_player);
        cur_node->print_to_log(log, alpha, beta);
        return cur_node;
    }
    //else if MAX's move
    else if(is_max){
        //return highest minimax_value of successors
        cur_node->print_to_log(log, alpha, beta);
        TreeItem *max_child_node = cur_node;
        int *next_moves = board.next_moves(player_num);
        int count = board.moves_available(player_num);
        for (int i=0; i<count; i++) {
            MancalaBoard nextBoard = MancalaBoard(board);
            bool bonus = nextBoard.make_move(player_num, next_moves[i]);
            int depth = cur_node->bonus ? current_depth : current_depth + 1;
            bool is_max = bonus;//stays true if a bonus move
            int next_player = player_num;
            if(!bonus){
                if(player_num == 1){
                    next_player = 2;
                }
                else{
                    next_player = 1;
                }
            }
            
            TreeItem *next_node;
            if(is_max){
                next_node = new TreeItem(is_max, INT32_MIN, player_num, next_moves[i], depth);
                g.addNode(next_node);
            }
            else{
                next_node = new TreeItem(is_max, INT32_MAX, player_num, next_moves[i], depth);
                g.addNode(next_node);
            }
            next_node->bonus = bonus;
            
            
            TreeItem *best_child = minimax_value_ab(nextBoard, depth, max_depth, next_player, is_max, next_node, root_player, log, alpha, beta, g);
            if(bonus){//if it's a part of a string of moves
                next_node->next_move = best_child;
            }
            
            if(!max_child_node){
                max_child_node = next_node;
                cur_node->value = max_child_node->value;
            }
            else if(next_node->value > max_child_node->value){
                max_child_node = next_node;
                cur_node->value = max_child_node->value;
            }
            if(max_child_node->value >= beta){
                cur_node->print_to_log(log, alpha, beta);
                delete [] next_moves;
                return max_child_node;
            }
            if(max_child_node->value > alpha){
                alpha = max_child_node->value;
            }
            cur_node->print_to_log(log, alpha, beta);
        }
        delete [] next_moves;
        return max_child_node;
    }
    //else (i.e. MIN's move)
    else{
        //return lowest minimax_value of successors
        cur_node->print_to_log(log, alpha, beta);
        TreeItem *min_child_node = cur_node;
        int *next_moves = board.next_moves(player_num);
        int count = board.moves_available(player_num);
        for (int i=0; i<count; i++) {
            MancalaBoard nextBoard = MancalaBoard(board);
            bool bonus = nextBoard.make_move(player_num, next_moves[i]);
            int depth = cur_node->bonus ? current_depth : current_depth + 1;
            bool is_max = !bonus;//MIN && bonus -> false,  MIN && !bonus -> true
            int next_player = player_num;
            if(!bonus){
                if(player_num == 1){
                    next_player = 2;
                }
                else{
                    next_player = 1;
                }
            }
            
            TreeItem *next_node;
            if(is_max){
                next_node = new TreeItem(is_max, INT32_MIN, player_num, next_moves[i], depth);
                g.addNode(next_node);
            }
            else{
                next_node = new TreeItem(is_max, INT32_MAX, player_num, next_moves[i], depth);
                g.addNode(next_node);
            }
            next_node->bonus = bonus;
            
            //next_node->print_to_log(log, print);
            TreeItem *best_child = minimax_value_ab(nextBoard, depth, max_depth, next_player, is_max, next_node, root_player, log, alpha, beta, g);
            if(bonus){//if it's a part of a string of moves
                next_node->next_move = best_child;
            }
            next_node->value = best_child->value;
            if(!min_child_node){
                min_child_node = next_node;
                cur_node->value = min_child_node->value;
            }
            else if(next_node->value < min_child_node->value){
                min_child_node = next_node;
                cur_node->value = min_child_node->value;
            }
            
            if(min_child_node->value <= alpha){
                cur_node -> print_to_log(log, alpha, beta);
                delete [] next_moves;
                return min_child_node;
            }
            if(min_child_node->value < beta){
                beta = min_child_node->value;
            }
            cur_node -> print_to_log(log, alpha, beta);
        }
        delete [] next_moves;
        return min_child_node;
    }
}