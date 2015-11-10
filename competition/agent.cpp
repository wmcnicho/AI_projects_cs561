//
//  agent.cpp
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
#include <chrono>
#include <ctime>

#include "MancalaBoard.h"
#include "TreeItem.h"
#include "GameTree.h"

class MancalaBoard;
struct TreeItem;
class GameTree;

void run_agent(MancalaBoard &initialBoard, int agent_player_num, double time_remaining);
TreeItem *my_minimax_value(MancalaBoard &board, GameTree &g, int current_depth, int max_depth, int player_num, bool is_max, TreeItem *cur_node, int agent_player_num, int alpha, int beta);


void run_ab(int p, std::vector<int>&p2_state, std::vector<int>&p1_state, int p2S, int p1S, int depth, std::ofstream &output, std::ofstream &log);
TreeItem *minimax_value_ab(MancalaBoard& board, int current_depth, int max_depth, int player_num, bool is_max, TreeItem* cur_node, int root_player, std::ofstream &log, int alpha, int beta, GameTree &g);


int main(int argc, char *argv[]){
    /* Input file for competition
     <task = 4 for competition>
     <Your player: 1 or 2>
     <your CPU time left in seconds: floating point number>
     <Board state for player-2>
     <Board state for player-1>
     <#stones in player-2’s mancala>
     <#stones in player-1’s mancala>
     */
    std::chrono::time_point<std::chrono::high_resolution_clock> program_start;
    program_start = std::chrono::high_resolution_clock::now();
    
    int task_num;
    int player_num;
    double cpu_time;
    //top player is player 2, bottom is player 1
    std::vector<int> top_player;
    std::vector<int> bot_player;
    int p1_score;
    int p2_score;
    
    std::ifstream input_file("input.txt");
    if(!input_file.is_open()){
        std::cout << "No input file found" << std::endl;
        return 1;
    }
    
    input_file >> task_num;
    input_file.ignore(INT_MAX, '\n');
    input_file >> player_num;
    input_file.ignore(INT_MAX, '\n');
    input_file >> cpu_time;
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
    
    if(task_num != 4){
        std::cout << "Error in input file or file reader." << std::endl;
        return 1;
    }
    
    MancalaBoard initialBoard = MancalaBoard(top_player, bot_player, p2_score, p1_score);
    
    std::chrono::time_point<std::chrono::high_resolution_clock> alg_start, end;
    alg_start = std::chrono::high_resolution_clock::now();
    run_agent(initialBoard, player_num, cpu_time);
    end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double> total_seconds = end-program_start;
    std::chrono::duration<double> io_time = alg_start-program_start;
    std::chrono::duration<double> computation_time = end-alg_start;
    std::cout <<  "Total elapsed time: " << total_seconds.count() << "s\n";
    std::cout <<  "I/O time: " << io_time.count() << "s\n";
    std::cout <<  "Computation time: " << computation_time.count() << "s\n";
    
    
    
    return 0;
}

void run_agent(MancalaBoard &initialBoard, int agent_player_num, double time_remaining){
    GameTree tree = GameTree();
    TreeItem *root = new TreeItem("root");
    tree.addChild(NULL, root);
    tree.addNode(root);
    
    TreeItem *best_move = root;
    
    int *next_moves = initialBoard.next_moves(agent_player_num);
    int count = initialBoard.moves_available(agent_player_num);
    
    //calculate time per branch
    
    
    //What should max depth be? Maybe a function of the current score.
    int max_depth = 10;
    
    int alpha = INT32_MIN;
    int beta = INT32_MAX;
    
    for (int i=0; i < count; i++) {
        MancalaBoard traverseBoard = initialBoard;
        bool bonus = traverseBoard.make_move(agent_player_num, next_moves[i]);
        int cur_depth = 1;
        bool is_max = bonus;
        int next_player = agent_player_num;
        if(!bonus){
            if(agent_player_num == 1){
                next_player = 2;
            }
            else{
                next_player = 1;
            }
        }
        TreeItem *next_node;
        if(is_max){
            next_node = new TreeItem(is_max, INT32_MIN, agent_player_num, next_moves[i], cur_depth);
            tree.addNode(next_node);
        }
        else{
            next_node = new TreeItem(is_max, INT32_MAX, agent_player_num, next_moves[i], cur_depth);
            tree.addNode(next_node);
        }
        next_node->bonus = bonus;
        TreeItem *best_child = my_minimax_value(traverseBoard, tree, cur_depth, max_depth, next_player, is_max, next_node, agent_player_num, alpha, beta);//Add additional value for time
        //minimax_value_ab(traverseBoard, cur_depth, depth, next_player, is_max, next_node, p, log, alpha, beta, tree);
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
    }
    delete [] next_moves;
    //at this point best_move should correspond to the TreeItem of the best move
    
    std::ofstream next_move("output.txt");
    TreeItem *move = best_move;
    while(move != NULL){
        std::cout << move->move_name << std::endl;
        next_move << move->move_name << std::endl;
        //also write to output file
        move = move->next_move;
    }
    
}
TreeItem *my_minimax_value(MancalaBoard &board, GameTree &g, int current_depth, int max_depth, int player_num, bool is_max, TreeItem *cur_node, int agent_player_num, int alpha, int beta){
    
    if (!cur_node->bonus && (board.moves_available(player_num) == 0 || current_depth >= max_depth) ){
        cur_node->value = board.util_value(player_num, agent_player_num);
        return cur_node;
    }
    //else if MAX's move
    else if(is_max){
        //return highest minimax_value of successors
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
            
            
            TreeItem *best_child = my_minimax_value(nextBoard, g, depth, max_depth, next_player, is_max, next_node, agent_player_num, alpha, beta);
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
                delete [] next_moves;
                return max_child_node;
            }
            if(max_child_node->value > alpha){
                alpha = max_child_node->value;
            }
        }
        delete [] next_moves;
        return max_child_node;
    }
    //else (i.e. MIN's move)
    else{
        //return lowest minimax_value of successors
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
            TreeItem *best_child = my_minimax_value(nextBoard, g, depth, max_depth, next_player, is_max, next_node, agent_player_num, alpha, beta);
            
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
                delete [] next_moves;
                return min_child_node;
            }
            if(min_child_node->value < beta){
                beta = min_child_node->value;
            }
        }
        delete [] next_moves;
        return min_child_node;
    }
    
}

//void run_ab(int p, std::vector<int>&p2_state, std::vector<int>&p1_state, int p2S, int p1S, int depth, std::ofstream &next_state, std::ofstream &log){
//    
//    //std::cout << "Node,Depth,Value,Alpha,Beta" << std::endl;
//    log << "Node,Depth,Value,Alpha,Beta" << std::endl;
//    
//    //TODO clean up all the TreeItem memory
//    GameTree tree = GameTree();
//    TreeItem *root = new TreeItem("root");
//    tree.addChild(NULL, root);
//    tree.addNode(root);
//    
//    TreeItem *best_move = root;
//    MancalaBoard initialBoard = MancalaBoard(p2_state, p1_state, p2S, p1S);
//    
//    int *next_moves = initialBoard.next_moves(p);
//    int count = initialBoard.moves_available(p);
//    int alpha = INT32_MIN;
//    int beta = INT32_MAX;
//    root->print_to_log(log, alpha, beta);
//    for (int i=0; i < count; i++) {
//        MancalaBoard traverseBoard = initialBoard;
//        bool bonus = traverseBoard.make_move(p, next_moves[i]);
//        int cur_depth = 1;
//        bool is_max = bonus;
//        int next_player = p;
//        if(!bonus){
//            if(p == 1){
//                next_player = 2;
//            }
//            else{
//                next_player = 1;
//            }
//        }
//        TreeItem *next_node;
//        if(is_max){
//            next_node = new TreeItem(is_max, INT32_MIN, p, next_moves[i], cur_depth);
//            tree.addNode(next_node);
//        }
//        else{
//            next_node = new TreeItem(is_max, INT32_MAX, p, next_moves[i], cur_depth);
//            tree.addNode(next_node);
//        }
//        next_node->bonus = bonus;
//        TreeItem *best_child = minimax_value_ab(traverseBoard, cur_depth, depth, next_player, is_max, next_node, p, log, alpha, beta, tree);
//        if(bonus){//if it's a part of a string of moves, needs fixing
//            next_node->next_move = best_child;
//        }
//        next_node->value = best_child->value;
//        if(next_node->value > best_move->value){
//            best_move = next_node;
//            root->value = best_move->value;
//        }
//        if(best_move->value >= beta){
//            break;//found! this should never hit
//        }
//        if(best_move->value > alpha){
//            alpha = best_move->value;
//        }
//        
//        root->print_to_log(log, alpha, beta);
//    }
//    delete [] next_moves;
//    //at this point best_move should correspond to the TreeItem of the best move
//    
//    //Make the series of moves to update the board to the new state
//    initialBoard.make_move(best_move->player_num, best_move->move_index);
//    TreeItem *move = best_move->next_move;
//    while(move != NULL){
//        initialBoard.make_move(move->player_num, move->move_index);
//        move = move->next_move;
//    }
//    
//    initialBoard.print_to_file(next_state);
//}
//
//TreeItem *minimax_value_ab(MancalaBoard& board, int current_depth, int max_depth, int player_num, bool is_max, TreeItem* cur_node, int root_player, std::ofstream &log, int alpha, int beta, GameTree &g){
//    if (!cur_node->bonus && (board.moves_available(player_num) == 0 || current_depth >= max_depth) ){
//        cur_node->value = board.util_value(player_num, root_player);
//        cur_node->print_to_log(log, alpha, beta);
//        return cur_node;
//    }
//    //else if MAX's move
//    else if(is_max){
//        //return highest minimax_value of successors
//        cur_node->print_to_log(log, alpha, beta);
//        TreeItem *max_child_node = cur_node;
//        int *next_moves = board.next_moves(player_num);
//        int count = board.moves_available(player_num);
//        for (int i=0; i<count; i++) {
//            MancalaBoard nextBoard = MancalaBoard(board);
//            bool bonus = nextBoard.make_move(player_num, next_moves[i]);
//            int depth = cur_node->bonus ? current_depth : current_depth + 1;
//            bool is_max = bonus;//stays true if a bonus move
//            int next_player = player_num;
//            if(!bonus){
//                if(player_num == 1){
//                    next_player = 2;
//                }
//                else{
//                    next_player = 1;
//                }
//            }
//            
//            TreeItem *next_node;
//            if(is_max){
//                next_node = new TreeItem(is_max, INT32_MIN, player_num, next_moves[i], depth);
//                g.addNode(next_node);
//            }
//            else{
//                next_node = new TreeItem(is_max, INT32_MAX, player_num, next_moves[i], depth);
//                g.addNode(next_node);
//            }
//            next_node->bonus = bonus;
//            
//            
//            TreeItem *best_child = minimax_value_ab(nextBoard, depth, max_depth, next_player, is_max, next_node, root_player, log, alpha, beta, g);
//            if(bonus){//if it's a part of a string of moves
//                next_node->next_move = best_child;
//            }
//            
//            if(!max_child_node){
//                max_child_node = next_node;
//                cur_node->value = max_child_node->value;
//            }
//            else if(next_node->value > max_child_node->value){
//                max_child_node = next_node;
//                cur_node->value = max_child_node->value;
//            }
//            if(max_child_node->value >= beta){
//                cur_node->print_to_log(log, alpha, beta);
//                delete [] next_moves;
//                return max_child_node;
//            }
//            if(max_child_node->value > alpha){
//                alpha = max_child_node->value;
//            }
//            cur_node->print_to_log(log, alpha, beta);
//        }
//        delete [] next_moves;
//        return max_child_node;
//    }
//    //else (i.e. MIN's move)
//    else{
//        //return lowest minimax_value of successors
//        cur_node->print_to_log(log, alpha, beta);
//        TreeItem *min_child_node = cur_node;
//        int *next_moves = board.next_moves(player_num);
//        int count = board.moves_available(player_num);
//        for (int i=0; i<count; i++) {
//            MancalaBoard nextBoard = MancalaBoard(board);
//            bool bonus = nextBoard.make_move(player_num, next_moves[i]);
//            int depth = cur_node->bonus ? current_depth : current_depth + 1;
//            bool is_max = !bonus;//MIN && bonus -> false,  MIN && !bonus -> true
//            int next_player = player_num;
//            if(!bonus){
//                if(player_num == 1){
//                    next_player = 2;
//                }
//                else{
//                    next_player = 1;
//                }
//            }
//            
//            TreeItem *next_node;
//            if(is_max){
//                next_node = new TreeItem(is_max, INT32_MIN, player_num, next_moves[i], depth);
//                g.addNode(next_node);
//            }
//            else{
//                next_node = new TreeItem(is_max, INT32_MAX, player_num, next_moves[i], depth);
//                g.addNode(next_node);
//            }
//            next_node->bonus = bonus;
//            
//            //next_node->print_to_log(log, print);
//            TreeItem *best_child = minimax_value_ab(nextBoard, depth, max_depth, next_player, is_max, next_node, root_player, log, alpha, beta, g);
//            if(bonus){//if it's a part of a string of moves
//                next_node->next_move = best_child;
//            }
//            next_node->value = best_child->value;
//            if(!min_child_node){
//                min_child_node = next_node;
//                cur_node->value = min_child_node->value;
//            }
//            else if(next_node->value < min_child_node->value){
//                min_child_node = next_node;
//                cur_node->value = min_child_node->value;
//            }
//            
//            if(min_child_node->value <= alpha){
//                cur_node -> print_to_log(log, alpha, beta);
//                delete [] next_moves;
//                return min_child_node;
//            }
//            if(min_child_node->value < beta){
//                beta = min_child_node->value;
//            }
//            cur_node -> print_to_log(log, alpha, beta);
//        }
//        delete [] next_moves;
//        return min_child_node;
//    }
//}