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
#include <assert.h>
#include <stdio.h>

#include "MancalaBoard.h"
#include "TreeItem.h"
#include "GameTree.h"

//class MancalaBoard;
//struct TreeItem;
//class GameTree;

std::vector<int> run_agent(MancalaBoard &initialBoard, int agent_player_num, double time_remaining, std::chrono::time_point<std::chrono::high_resolution_clock> &start_time, bool output);
TreeItem *my_minimax_value(MancalaBoard &board, GameTree &g, int current_depth, int max_depth, int player_num, bool is_max, TreeItem *cur_node, int agent_player_num, int alpha, int beta, double traverse_time, std::chrono::time_point<std::chrono::high_resolution_clock> &start_time);
int calculate_depth(MancalaBoard &initialBoard, int agent_player_num, double time_remaining, int num_of_operators);
double calculate_time(MancalaBoard &initialBoard, int agent_player_num, double time_remaining, int num_of_operators);

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
    srand(time(nullptr));
    std::chrono::time_point<std::chrono::high_resolution_clock> program_start;
    program_start = std::chrono::high_resolution_clock::now();
    int selection;
    
    std::cout << "Game(1) or Competition mode(~1)?\n" << std::endl;
    std::cin >> selection;
    
    if(selection==1){
        
        int player_turn;
        int bot_turn;
        
        std::cout << "Guess high or low" << std::endl;
        std::string choice;
        std::cin >> choice;
        while(strcmp(choice.c_str(), "high") * strcmp(choice.c_str(), "low") != 0 ){
            std::cout << "Please select high or low (above or below 50)";
            std::cin >>choice;
        }
        
        int random = rand() % 100 + 1;
        std::cout << "Random roll is: " << random << std::endl;
        
        if(strcmp(choice.c_str(), "high") == 0){
            if(random >= 50){
                std::cout << "Player guesses right and goes first" << std::endl;
                player_turn = 1;
            }
            else{
                std::cout << "Player guesses wrong and goes second" << std::endl;
                player_turn = 2;
            }
        }
        else{
            if(random < 50){
                std::cout << "Player guesses right and goes first" << std::endl;
                player_turn = 1;
            }
            else{
                std::cout << "Player guesses wrong and goes second" << std::endl;
                player_turn = 2;
            }
        }
        bot_turn = (player_turn == 1) ? 2 : 1;
        
        //make a board
        int board_start[6] = {4,4,4,4,4,4};
        
        std::vector<int> p1_state(board_start, board_start+sizeof(board_start) / sizeof(board_start[0]));
        MancalaBoard gameBoard = MancalaBoard(p1_state, p1_state, 0, 0);
        
        //gameBoard.make_move(2, 0);
        
        assert(gameBoard.checkMoveValid(0, 1) == true);
        assert(gameBoard.checkMoveValid(2, 1) == true);
        assert(gameBoard.checkMoveValid(6, 1) == false);
        
        
        //if player goes first query first move
        bool bonus = true;
        int player_move;
        std::cout << gameBoard.toString() << std::endl;
        while(bonus && player_turn == 1){
            std::cout << "Please select a move (0-indexed array for your side)\n";
            std::cin >>player_move;
            while(!gameBoard.checkMoveValid(player_move, player_turn)){
                std::cout << "Please enter a valid move" << std::endl;
                std::cin >> player_move;
            }
            bonus = gameBoard.make_move(player_turn, player_move);
            std::cout << "\nBoard after your move:\n" << gameBoard.toString() << std::endl;
        }
        
        //While loop until game is over
        while(!gameBoard.checkGameOver(bot_turn)){
            //Decide a move
            std::chrono::time_point<std::chrono::high_resolution_clock> decision_start;
            program_start = std::chrono::high_resolution_clock::now();
            std::vector<int> bot_moves = run_agent(gameBoard, bot_turn, 100, decision_start, false);
            //Update board based off of decision
            for (int i=0; i<bot_moves.size(); ++i) {
                gameBoard.make_move(bot_turn, bot_moves[i]);
            }
            //Query user for move
            if(gameBoard.checkGameOver(player_turn)){
                break;
            }
            std::cout << gameBoard.toString() << std::endl;
            bool bonus = true;
            while(bonus){
                std::cout << "Please select a move (0-indexed array for your side)\n";
                std::cin >>player_move;
                while(!gameBoard.checkMoveValid(player_move, player_turn)){
                    std::cout << "Please enter a valid move" << std::endl;
                    std::cin >> player_move;
                }
                bonus = gameBoard.make_move(player_turn, player_move);
                std::cout << "Board after your move:\n" << gameBoard.toString() << std::endl;
            }
        }
        
        std::cout << "Final board:\n" << gameBoard.toString() << std::endl;
        //Display winner
        int winner = gameBoard.p1_mancala > gameBoard.p2_mancala ? 1 : 2;
        if(winner == player_turn){
            std::cout << "Player wins!" << std::endl;
        }
        else{
            std::cout << "Bot wins!" << std::endl;
        }

    }
    else{
        
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
        run_agent(initialBoard, player_num, cpu_time, alg_start, true);
        end = std::chrono::high_resolution_clock::now();
        
        std::chrono::duration<double> total_seconds = end-program_start;
        std::chrono::duration<double> io_time = alg_start-program_start;
        std::chrono::duration<double> computation_time = end-alg_start;
        std::cout <<  "Total elapsed time: " << total_seconds.count() << "s\n";
        std::cout <<  "I/O time: " << io_time.count() << "s\n";
        std::cout <<  "Computation time: " << computation_time.count() << "s\n";
        
    }
    
    
    return 0;
}

std::vector<int> run_agent(MancalaBoard &initialBoard, int agent_player_num, double time_remaining, std::chrono::time_point<std::chrono::high_resolution_clock> &start_time, bool output){
    GameTree tree = GameTree();
    TreeItem *root = new TreeItem("root");
    tree.addChild(NULL, root);
    tree.addNode(root);
    
    TreeItem *best_move = root;
    
    int *next_moves = initialBoard.next_moves(agent_player_num);
    int count = initialBoard.moves_available(agent_player_num);
    
    //calculate time per branch
    double search_time = calculate_time(initialBoard, agent_player_num, time_remaining, count);
    
    //What should max depth be? Maybe a function of the current score.
    int max_depth = calculate_depth(initialBoard, agent_player_num, time_remaining, count);
    
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
        TreeItem *best_child = my_minimax_value(traverseBoard, tree, cur_depth, max_depth, next_player, is_max, next_node, agent_player_num, alpha, beta, search_time, start_time);//Add additional value for time
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
    std::vector<int> moves;
    if(output){
        std::ofstream next_move("output.txt");
        TreeItem *move = best_move;
        while(move != NULL){
            std::cout << move->move_name << std::endl;
            next_move << move->move_name << std::endl;
            //also write to output file
            move = move->next_move;
        }
        std::cout << "Next move written to ouput.txt" << std::endl;
    }
    else{
        std::cout << "Bot decides: " << std::endl;
        TreeItem *move = best_move;
        while(move != NULL){
            moves.push_back(move->move_index);
            std::cout << move->move_index;
            //also write to output file
            move = move->next_move;
        }
        std::cout << std::endl;
        
    }
    return moves;
}
TreeItem *my_minimax_value(MancalaBoard &board, GameTree &g, int current_depth, int max_depth, int player_num, bool is_max, TreeItem *cur_node, int agent_player_num, int alpha, int beta, double traverse_time, std::chrono::time_point<std::chrono::high_resolution_clock> &start_time){
    std::chrono::time_point<std::chrono::high_resolution_clock> current_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> seconds_searching = start_time - current_time;
    double test = seconds_searching.count();
    if (!cur_node->bonus && (seconds_searching.count() >= traverse_time || board.moves_available(player_num) == 0 || current_depth >= max_depth) ){
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
            
            
            TreeItem *best_child = my_minimax_value(nextBoard, g, depth, max_depth, next_player, is_max, next_node, agent_player_num, alpha, beta, traverse_time, start_time);
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
            TreeItem *best_child = my_minimax_value(nextBoard, g, depth, max_depth, next_player, is_max, next_node, agent_player_num, alpha, beta, traverse_time, start_time);
            
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

int calculate_depth(MancalaBoard &initialBoard, int agent_player_num, double time_remaining, int num_of_operators){
    int total_score = initialBoard.p1_mancala + initialBoard.p2_mancala;
    
    double score_to_remain_ratio;
    if(total_score > 0){
        score_to_remain_ratio = static_cast<double>(initialBoard.remaining_stones()/total_score);
    }
    else{
        score_to_remain_ratio = 5;
    }
    
    if(score_to_remain_ratio > 1.5 ){//early game
        return 7;
    }
    else{
        return 10;
    }
}

double calculate_time(MancalaBoard &initialBoard, int agent_player_num, double time_remaining, int num_of_operators){
//    double stones_remaining = static_cast<double>(initialBoard.remaining_stones());
//    double cells = initialBoard.cells_per_player();
//    
//    double est_score_per_turn = (stones_remaining/cells) / (2 * cells + 1);
//    double estimated_moves_remaining = 0;
//    if(est_score_per_turn > 0){
//        estimated_moves_remaining = stones_remaining/est_score_per_turn;
//    }
//    
//    if(stones_remaining < 25.0 && stones_remaining > 0){//endgame scenario
//        double time_per_play  = (time_remaining/(stones_remaining*2));
//        return time_per_play/num_of_operators;
//    }
//    
//    double time_per_move =  0.001;
//    if(estimated_moves_remaining > 0){
//        time_per_move = (time_remaining / estimated_moves_remaining);
//    }
//    return time_per_move/num_of_operators;
    double time = 5.0;
    return time/static_cast<double>(num_of_operators);
}
